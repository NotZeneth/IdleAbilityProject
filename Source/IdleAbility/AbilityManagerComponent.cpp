#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"
#include "EnemyCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"

UAbilityManagerComponent::UAbilityManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAbilityManagerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAbilityManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Gestion des abilities auto
    for (int32 i = 0; i < EquippedAbilities.Num(); i++)
    {
        FAbilitySpec& Spec = EquippedAbilities[i];
        if (Spec.Ability && Spec.Ability->TriggerType == EAbilityTriggerType::Auto)
        {
            TryActivateAbility(i);
        }
    }

    // Gestion des effets persistants (DOT, buffs, debuffs…)
    TArray<ACustomCharacter*> TargetsToRemove;

    for (auto& Pair : ActiveEffects)
    {
        ACustomCharacter* Target = Pair.Key;
        if (!Target || !Target->IsAlive())
            continue;

        TArray<FAbilityEffectSpec>& Effects = Pair.Value;

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
    for (TActorIterator<ACustomCharacter> It(GetWorld()); It; ++It)
    {
        ACustomCharacter* C = *It;
        if (!C || C == Caster) continue;

        if (C->TeamId == Caster->TeamId) continue;

        Candidates.Add(C);
    }

    if (Ability->Targeting == EAbilityTargeting::RandomEnemies)
    {
        Algo::RandomShuffle(Candidates);
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

    for (TActorIterator<ACustomCharacter> It(GetWorld()); It; ++It)
    {
        ACustomCharacter* C = *It;
        if (!C || C == Origin) continue;
        if (!C->IsAlive()) continue;

        if (C->TeamId == Origin->TeamId) continue;

        if (FVector::DistSquared(C->GetActorLocation(), Origin->GetActorLocation()) <= RangeSq)
        {
            Out.Add(C);
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
        // Instantané
        EffectData->ApplyEffect(Context);
        return;
    }

    // Persistant -> appliquer une fois
    if (!EffectData->ApplyEffect(Context))
    {
        // Si l’effet a "raté" (Frenzy roll fail, etc.), on ne range rien
        return;
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
