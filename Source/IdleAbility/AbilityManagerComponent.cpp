#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"
#include "EnemyCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"
#include "WaveGameMode.h"

UAbilityManagerComponent::UAbilityManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAbilityManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    if (UWorld* World = GetWorld()) // Bon en vrai je devrais avoir le player qui le set et config mais flemme
    {
        GameModeRef = Cast<AWaveGameMode>(World->GetAuthGameMode());
    }
}

void UAbilityManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Gestion des abilities auto
    if (!GameModeRef || GameModeRef->EnemyList.Num() == 0)
        return; // Les ability se lancent pas s'il y a pas d'ennemi 

    for (int32 i = 0; i < EquippedAbilities.Num(); i++)
    {
        FAbilitySpec& Spec = EquippedAbilities[i];
        if (Spec.Ability && Spec.isAutoCast)
        {
            TryActivateAbility(i);
        }
    }

    // ============================================================
    // Gestion des effets persistants (DOT, buffs, debuffs…)
    // ============================================================

    TArray<ACustomCharacter*> Keys;
    ActiveEffects.GetKeys(Keys);   // On fige la liste avant d'itérer
    TArray<ACustomCharacter*> TargetsToRemove;

    for (ACustomCharacter* Target : Keys)
    {
        if (!Target || !Target->IsAlive())
            continue;

        TArray<FAbilityEffectSpec>* EffectsPtr = ActiveEffects.Find(Target);
        if (!EffectsPtr)
            continue;

        TArray<FAbilityEffectSpec>& Effects = *EffectsPtr;

        Effects.RemoveAll([&](FAbilityEffectSpec& Spec)
            {
                const UAbilityEffectData* Data = Spec.EffectData;
                if (!Data) return true;

                const float TickEvery = FMath::Max(0.01f, Data->TickInterval);

                Spec.TimeRemaining -= DeltaTime;
                Spec.TimeSinceLastTick += DeltaTime;

                // Tick périodique
                if (TickEvery > 0.f)
                {
                    while (Spec.TimeSinceLastTick >= TickEvery && Spec.TimeRemaining > 0.f)
                    {
                        Spec.TimeSinceLastTick -= TickEvery;
                        Data->ApplyEffect(Spec.Context);
                    }
                }

                // Expiration
                if (Spec.TimeRemaining <= 0.f)
                {
                    // Déclenche les sous-effets OnExpire
                    for (const UAbilityEffectData* Sub : Data->SubEffects)
                    {
                        if (Sub && Sub->TriggerPhase == EEffectTriggerPhase::OnExpire)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("[Expire] Déclenche sous-effet %s sur %s"),
                                *Sub->GetClass()->GetName(),
                                Spec.Context.Target ? *Spec.Context.Target->GetName() : TEXT("null"));

                            Sub->ApplyEffect(Spec.Context);
                        }
                    }

                    UE_LOG(LogTemp, Log, TEXT("[Manager] Expiration de %s sur %s"),
                        *Data->GetClass()->GetName(),
                        Spec.Context.Target ? *Spec.Context.Target->GetName() : TEXT("null"));

                    return true; // supprimer cet effet
                }

                return false; // garder
            });

        if (Effects.Num() == 0)
        {
            TargetsToRemove.Add(Target); // suppression différée
        }
    }

    // Nettoyage des cibles vides après l’itération (safe)
    for (ACustomCharacter* Tgt : TargetsToRemove)
    {
        ActiveEffects.Remove(Tgt);
    }

    // ============================================================
    // Suppression différée des entités mortes
    // ============================================================
    if (PendingRemovals.Num() > 0)
    {
        for (ACustomCharacter* Dead : PendingRemovals)
        {
            ActiveEffects.Remove(Dead);
        }
        PendingRemovals.Reset();
    }
}




bool UAbilityManagerComponent::IsAbilityReady(const FAbilitySpec& Spec) const
{
    return (Spec.Ability != nullptr) && (GetWorld()->TimeSeconds >= Spec.CooldownEndTime);
}

void UAbilityManagerComponent::TryActivateAbility(int32 AbilityIndex)
{
    if (!EquippedAbilities.IsValidIndex(AbilityIndex)) return;

    FAbilitySpec& Spec = EquippedAbilities[AbilityIndex];
    if (!IsAbilityReady(Spec)) return;

    ACustomCharacter* Caster = Cast<ACustomCharacter>(GetOwner());
    if (!Caster) return;

    ExecuteAbility(Spec);

    // Cooldown ajusté
    float BaseCd = Spec.Ability->Cooldown;
    float Scalar = Spec.CooldownScalar;
    float Cdr = 1.f - Caster->CooldownReduction;

    float FinalCooldown = BaseCd * Scalar * Cdr;
    if (FinalCooldown < 0.1f) FinalCooldown = 0.05f;

    Spec.CooldownEndTime = GetWorld()->TimeSeconds + FinalCooldown;
}

void UAbilityManagerComponent::ExecuteAbility(const FAbilitySpec& Spec)
{
    if (!Spec.Ability) return;

    ACustomCharacter* Caster = Cast<ACustomCharacter>(GetOwner());
    if (!Caster) return;

    // Trouver les cibles
    TArray<ACustomCharacter*> Targets;
    FindTargets(Spec.Ability, Caster, Targets);

    // Self-case
    if (Spec.Ability->Targeting == EAbilityTargeting::Self && Targets.Num() == 0)
    {
        Targets.Add(Caster);
    }

    if (Targets.Num() == 0 && Spec.Ability->Targeting != EAbilityTargeting::Self)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s a lancé %s mais aucune cible trouvée."),
            *Caster->GetName(), *Spec.Ability->AbilityName.ToString());
        return;
    }

    // Appliquer les effets OnCast
    for (UAbilityEffectData* EffectData : Spec.Ability->Effects)
    {
        if (!EffectData || EffectData->TriggerPhase != EEffectTriggerPhase::OnCast)
            continue;

        for (ACustomCharacter* Tgt : Targets)
        {
            FAbilityEffectContext Ctx;
            Ctx.Source = Caster;
            Ctx.Target = Tgt;
            Ctx.Ability = Spec.Ability;
            Ctx.Projectile = nullptr;

            ApplyEffectToTarget(EffectData, Ctx);
        }
    }
}

void UAbilityManagerComponent::FindTargets(const UAbilityData* Ability, ACustomCharacter* Caster, TArray<ACustomCharacter*>& OutTargets) const
{
    if (!Ability || !Caster) return;

    // Collecte brute des ennemis
    TArray<ACustomCharacter*> Candidates;
    if (GameModeRef)
    {
        for (AEnemyCharacter* Enemy : GameModeRef->EnemyList)
        {
            if (!Enemy || !Enemy->IsAlive()) continue;
            Candidates.Add(Enemy);
        }
    }

    if (Ability->Targeting == EAbilityTargeting::RandomEnemies)
    {
        Algo::RandomShuffle(Candidates); // bon ca c'est de la flmm mais ca existe donc why not hein
        const int32 N = FMath::Min(Ability->TargetCount, Candidates.Num());
        for (int32 i = 0; i < N; ++i)
        {
            OutTargets.Add(Candidates[i]);
        }
    }
    else if (Ability->Targeting == EAbilityTargeting::SingleNearestEnemy)
    {
        float BestDistSq = TNumericLimits<float>::Max();
        ACustomCharacter* Best = nullptr;

        for (ACustomCharacter* C : Candidates)
        {
            float d = FVector::DistSquared(C->GetActorLocation(), Caster->GetActorLocation());
            if (d < BestDistSq && d <= Ability->Range * Ability->Range)
            {
                BestDistSq = d;
                Best = C;
            }
        }
        if (Best) OutTargets.Add(Best);
    }
    else if (Ability->Targeting == EAbilityTargeting::AllEnemiesInRange)
    {
        for (ACustomCharacter* C : Candidates)
        {
            float d = FVector::DistSquared(C->GetActorLocation(), Caster->GetActorLocation());
            if (d <= Ability->Range * Ability->Range)
            {
                OutTargets.Add(C);
            }
        }
    }
}

void UAbilityManagerComponent::GetEnemiesInRange(const ACustomCharacter* Origin, float Range, TArray<ACustomCharacter*>& Out) const
{
    Out.Reset();
    if (!Origin) return;

    const float RangeSq = (Range <= 0.f) ? FLT_MAX : Range * Range;

    if (GameModeRef)
    {
        for (AEnemyCharacter* Enemy : GameModeRef->EnemyList)
        {
            if (!Enemy || !Enemy->IsAlive()) continue;

            const float DistSq = FVector::DistSquared(Enemy->GetActorLocation(), Origin->GetActorLocation());
            if (DistSq <= RangeSq)
            {
                Out.Add(Enemy);
            }
        }
    }
}

void UAbilityManagerComponent::OnEnemyKilled(AEnemyCharacter* DeadEnemy)
{
    if (!DeadEnemy) return;

    UE_LOG(LogTemp, Log, TEXT("[Manager] Cleanup différé des effets persistants sur %s"), *DeadEnemy->GetName());
    PendingRemovals.Add(DeadEnemy);
}


void UAbilityManagerComponent::ApplyEffectToTarget(const UAbilityEffectData* EffectData, const FAbilityEffectContext& Context)
{
    if (!EffectData) return;

    if (EffectData->Duration <= 0.f)
    {
        // Instantané (one-shot)
        EffectData->ApplyEffect(Context);
        return;
    }

    // Persistant (effet qui dure)
    if (EffectData->bTriggerOnApply)
    {
        // Appliquer tout de suite si l’effet le veut (DoT, Heal Over Time, etc.)
        if (!EffectData->ApplyEffect(Context))
        {
            return;
        }
    }

    FAbilityEffectSpec NewSpec(EffectData, Context);
    NewSpec.TimeRemaining = EffectData->Duration;

    // Reset timer après le tick initial
    NewSpec.TimeSinceLastTick = 0.f;

    ActiveEffects.FindOrAdd(Context.Target).Add(NewSpec);

    UE_LOG(LogTemp, Log, TEXT("[Manager] Effet persistant %s posé sur %s (Duration=%.2fs, Tick=%.2fs)"),
        *EffectData->GetClass()->GetName(),
        Context.Target ? *Context.Target->GetName() : TEXT("null"),
        NewSpec.TimeRemaining,
        EffectData->TickInterval);
}

// --- ResetAllEffectsAndCooldowns ---
// Cette fonction est appelée entre deux vagues d'ennemis (depuis le GameMode).
// Elle supprime tous les effets persistants (DoT, buffs, Frenzy, etc.)
// et réinitialise les cooldowns des compétences du joueur.
//
// Pourquoi ?
// Lorsqu'une nouvelle vague commence, on veut éviter que des effets de la
// vague précédente continuent de tick ou influencent la cadence d'attaque.
// Sans ce reset, des états orphelins (ex: Frenzy encore actif sur un ennemi mort)
// pourraient provoquer des comportements incohérents (spam d'attaques, cooldowns bloqués...).
//
// En vidant ActiveEffects et en remettant les CooldownEndTime à zéro,
// on garantit que le joueur repart d'un état "propre" à chaque nouvelle vague.

// Update : on trigger aussi le expire, typiquement que frenzy call unfrenzy
void UAbilityManagerComponent::ResetAllEffectsAndCooldowns()
{
    UE_LOG(LogTemp, Warning, TEXT("[Reset] START — ActiveEffects=%d targets"), ActiveEffects.Num());

    // 1) Forcer tous les OnExpire (ex: UnFrenzy / Unfreeze / UnWeaken…)
    //    On parcourt une copie des clés pour éviter toute modif pendant l'itération.
    TArray<ACustomCharacter*> Keys;
    ActiveEffects.GetKeys(Keys);

    for (ACustomCharacter* Target : Keys)
    {
        if (!Target) continue;

        TArray<FAbilityEffectSpec>* EffectsPtr = ActiveEffects.Find(Target);
        if (!EffectsPtr) continue;

        for (FAbilityEffectSpec& Spec : *EffectsPtr)
        {
            if (!Spec.EffectData) continue;

            for (const UAbilityEffectData* Sub : Spec.EffectData->SubEffects)
            {
                if (Sub && Sub->TriggerPhase == EEffectTriggerPhase::OnExpire)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[Reset] Force OnExpire %s -> Target=%s"),
                        *Sub->GetClass()->GetName(),
                        Spec.Context.Target ? *Spec.Context.Target->GetName() : TEXT("null"));

                    Sub->ApplyEffect(Spec.Context);
                }
            }
        }
    }

    // 2) Purge des effets persistants + files d’attente
    ActiveEffects.Empty();
    PendingRemovals.Empty();

    // 3) Reset des cooldowns ET—par sécurité—du CooldownScalar (anti-Frenzy bloqué)
    //    (si d'autres effets modifient le scalar un jour, on pourra l'affiner)
    for (FAbilitySpec& Spec : EquippedAbilities)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Reset] Before: %s  CDScalar=%.3f"),
            Spec.Ability ? *Spec.Ability->AbilityName.ToString() : TEXT("null"),
            Spec.CooldownScalar);

        Spec.CooldownEndTime = 0.f;
        Spec.CooldownScalar = 1.f;

        UE_LOG(LogTemp, Warning, TEXT("[Reset] After : %s  CDScalar=%.3f"),
            Spec.Ability ? *Spec.Ability->AbilityName.ToString() : TEXT("null"),
            Spec.CooldownScalar);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Reset] DONE"));
}
