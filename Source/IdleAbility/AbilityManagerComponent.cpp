// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"
#include "EnemyCharacter.h"
#include "PlayerCharacter.h"
#include "WaveGameMode.h"
#include "GameplayWidget.h"
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

    // en vrai je devrais avoir le player qui setup ca, mais flemme donc on le chope via le World
    if (UWorld* World = GetWorld())
    {
        GameModeRef = Cast<AWaveGameMode>(World->GetAuthGameMode());
    }

    // init des upgrades pour chaque ability equiped
    for (FAbilitySpec& Spec : EquippedAbilities)
    {
        if (Spec.Ability)
        {
            UpgradeLevelsByAbility.Add(Spec.Ability, FUpgradeLevels());
            Spec.bUnlocked = Spec.Ability->bUnlocked; // synchro avec la valeur du DA
        }
    }
}

void UAbilityManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // abilities auto s'il y a des enemies only
    if (!GameModeRef || GameModeRef->EnemyList.Num() == 0)
        return;

    for (int32 i = 0; i < EquippedAbilities.Num(); i++)
    {
        FAbilitySpec& Spec = EquippedAbilities[i];
        if (Spec.Ability && Spec.isAutoCast)
        {
            TryActivateAbility(i);
        }
    }

    // effets persistants
    TArray<ACustomCharacter*> Keys;
    ActiveEffects.GetKeys(Keys);
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

                // tick périodique
                if (TickEvery > 0.f)
                {
                    while (Spec.TimeSinceLastTick >= TickEvery && Spec.TimeRemaining > 0.f)
                    {
                        Spec.TimeSinceLastTick -= TickEvery;
                        Data->ApplyEffect(Spec.Context);
                    }
                }

                // expiration
                if (Spec.TimeRemaining <= 0.f)
                {
                    // sous effets OnExpire
                    for (const UAbilityEffectData* Sub : Data->SubEffects)
                    {
                        if (Sub && Sub->TriggerPhase == EEffectTriggerPhase::OnExpire)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("[Expire] Trigger sub-effect %s on %s"),
                                *Sub->GetClass()->GetName(),
                                Spec.Context.Target ? *Spec.Context.Target->GetName() : TEXT("null"));

                            Sub->ApplyEffect(Spec.Context);
                        }
                    }

                    UE_LOG(LogTemp, Log, TEXT("[Manager] Expire %s on %s"),
                        *Data->GetClass()->GetName(),
                        Spec.Context.Target ? *Spec.Context.Target->GetName() : TEXT("null"));

                    return true; // on supprime cet effet
                }

                return false; // on garde
            });

        if (Effects.Num() == 0)
        {
            TargetsToRemove.Add(Target);
        }
    }

    // clean up
    for (ACustomCharacter* Tgt : TargetsToRemove)
    {
        ActiveEffects.Remove(Tgt);
    }

    // clean up, on fait bc de copies de listes et tout pour eviter les erreurs, crash, iterration sur une array qu'on modifie etc
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

    // gestion du cooldown
    float BaseCd = Spec.Ability->Cooldown;

    const FUpgradeLevels* Levels = UpgradeLevelsByAbility.Find(Spec.Ability);
    const TArray<float>& Values = Spec.Ability->BaseUpgrades.Cooldown.EffectValues;

    if (Levels && Values.Num() > 0)
    {
        const int32 Level = Levels->CooldownLevel;
        BaseCd = Values.IsValidIndex(Level) ? Values[Level] : Values.Last();
    }

    float FinalCooldown = BaseCd * Spec.CooldownScalar;
    FinalCooldown = FMath::Max(0.05f, FinalCooldown);
    Spec.CooldownEndTime = GetWorld()->TimeSeconds + FinalCooldown;
}

void UAbilityManagerComponent::ExecuteAbility(const FAbilitySpec& Spec)
{
    if (!Spec.Ability) return;

    ACustomCharacter* Caster = Cast<ACustomCharacter>(GetOwner());
    if (!Caster) return;

    TArray<ACustomCharacter*> Targets;
    FindTargets(Spec.Ability, Caster, Targets);

    // si Self et pas de cible, on se l’applique à soi-même
    // ! Jamais retested car au final on a aucune ability en self, c'est du legacy pas retouché
    if (Spec.Ability->Targeting == EAbilityTargeting::Self && Targets.Num() == 0)
    {
        Targets.Add(Caster);
    }

    if (Targets.Num() == 0 && Spec.Ability->Targeting != EAbilityTargeting::Self)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s a lance %s mais aucune cible trouvee"),
            *Caster->GetName(), *Spec.Ability->AbilityName.ToString());
        return;
    }

    // on applique les effets OnCast
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

    UE_LOG(LogTemp, Log, TEXT("[Manager] Cleanup des effets sur %s"), *DeadEnemy->GetName());
    PendingRemovals.Add(DeadEnemy);
}

void UAbilityManagerComponent::ApplyEffectToTarget(const UAbilityEffectData* EffectData, const FAbilityEffectContext& Context)
{
    if (!EffectData) return;

    if (EffectData->Duration <= 0.f)
    {
        // instantané
        EffectData->ApplyEffect(Context);
        return;
    }

    // persistant
    if (EffectData->bTriggerOnApply)
    {
        if (!EffectData->ApplyEffect(Context))
        {
            return;
        }
    }

    FAbilityEffectSpec NewSpec(EffectData, Context);
    NewSpec.TimeRemaining = EffectData->Duration;
    NewSpec.TimeSinceLastTick = 0.f;

    ActiveEffects.FindOrAdd(Context.Target).Add(NewSpec);

    UE_LOG(LogTemp, Log, TEXT("[Manager] Effet persistant %s sur %s (%.2fs, Tick=%.2fs)"),
        *EffectData->GetClass()->GetName(),
        Context.Target ? *Context.Target->GetName() : TEXT("null"),
        NewSpec.TimeRemaining,
        EffectData->TickInterval);
}

void UAbilityManagerComponent::ResetAllEffectsAndCooldowns()
{
    UE_LOG(LogTemp, Warning, TEXT("[Reset] START - %d targets"), ActiveEffects.Num());

    // on force tous les OnExpire (ex: UnFrenzy / Unfreeze / UnWeaken)
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
                    UE_LOG(LogTemp, Warning, TEXT("[Reset] Force OnExpire %s -> %s"),
                        *Sub->GetClass()->GetName(),
                        Spec.Context.Target ? *Spec.Context.Target->GetName() : TEXT("null"));

                    Sub->ApplyEffect(Spec.Context);
                }
            }
        }
    }

    ActiveEffects.Empty();
    PendingRemovals.Empty();

    // reset des cooldowns et scalars
    for (FAbilitySpec& Spec : EquippedAbilities)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Reset] Before: %s CDScalar=%.3f"),
            Spec.Ability ? *Spec.Ability->AbilityName.ToString() : TEXT("null"),
            Spec.CooldownScalar);

        Spec.CooldownEndTime = 0.f;
        Spec.CooldownScalar = 1.f;

        UE_LOG(LogTemp, Warning, TEXT("[Reset] After : %s CDScalar=%.3f"),
            Spec.Ability ? *Spec.Ability->AbilityName.ToString() : TEXT("null"),
            Spec.CooldownScalar);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Reset] DONE"));
}

void UAbilityManagerComponent::UpgradeAbility(UAbilityData* Ability, FString UpgradeName)
{
    if (!Ability) return;

    FAbilityUpgradeSet& Data = const_cast<FAbilityUpgradeSet&>(Ability->BaseUpgrades);
    FUpgradeStat* Stat = nullptr;

    if (UpgradeName == "Damage") Stat = &Data.Damage;
    else if (UpgradeName == "Cooldown") Stat = &Data.Cooldown;
    else if (UpgradeName == "MultishotChance") Stat = &Data.MultishotChance;
    else if (UpgradeName == "MultishotAmount") Stat = &Data.MultishotAmount;
    else if (UpgradeName == "BounceChance") Stat = &Data.BounceChance;
    else if (UpgradeName == "BounceAmount") Stat = &Data.BounceAmount;
    else if (UpgradeName == "FrenzyChance") Stat = &Data.FrenzyChance;

    if (!Stat || Stat->EffectValues.Num() == 0) return;

    FUpgradeLevels* Levels = UpgradeLevelsByAbility.Find(Ability);
    if (!Levels) return;

    int32* LevelPtr = nullptr;
    if (UpgradeName == "Damage") LevelPtr = &Levels->DamageLevel;
    else if (UpgradeName == "Cooldown") LevelPtr = &Levels->CooldownLevel;
    else if (UpgradeName == "MultishotChance") LevelPtr = &Levels->MultishotChanceLevel;
    else if (UpgradeName == "MultishotAmount") LevelPtr = &Levels->MultishotAmountLevel;
    else if (UpgradeName == "BounceChance") LevelPtr = &Levels->BounceChanceLevel;
    else if (UpgradeName == "BounceAmount") LevelPtr = &Levels->BounceAmountLevel;
    else if (UpgradeName == "FrenzyChance") LevelPtr = &Levels->FrenzyChanceLevel;

    if (!LevelPtr) return;

    int32 CurrentLevel = *LevelPtr;
    int32 MaxLevel = Stat->EffectValues.Num();

    if (CurrentLevel >= MaxLevel)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Upgrade] %s : %s max level (%d)"),
            *Ability->AbilityName.ToString(), *UpgradeName, CurrentLevel);
        return;
    }

    float Cost = Stat->UpgradeCosts.IsValidIndex(CurrentLevel) ? Stat->UpgradeCosts[CurrentLevel] : 0.f;

    if (PlayerRef->CurrentGold < Cost)
        return;

    PlayerRef->AddGold(-Cost);
    *LevelPtr += 1;
}

float UAbilityManagerComponent::GetUpgradeValue(const UAbilityData* Ability, const FString& UpgradeName) const
{
    if (!Ability) return 0.f;
    const FUpgradeLevels* Levels = UpgradeLevelsByAbility.Find(const_cast<UAbilityData*>(Ability));
    if (!Levels) return 0.f;

    const FAbilityUpgradeSet& Data = Ability->BaseUpgrades;

    int32 Level = 0;
    const TArray<float>* Values = nullptr;

    if (UpgradeName == "Damage") { Level = Levels->DamageLevel;          Values = &Data.Damage.EffectValues; }
    else if (UpgradeName == "Cooldown") { Level = Levels->CooldownLevel;        Values = &Data.Cooldown.EffectValues; }
    else if (UpgradeName == "MultishotChance") { Level = Levels->MultishotChanceLevel; Values = &Data.MultishotChance.EffectValues; }
    else if (UpgradeName == "MultishotAmount") { Level = Levels->MultishotAmountLevel; Values = &Data.MultishotAmount.EffectValues; }
    else if (UpgradeName == "BounceChance") { Level = Levels->BounceChanceLevel;    Values = &Data.BounceChance.EffectValues; }
    else if (UpgradeName == "BounceAmount") { Level = Levels->BounceAmountLevel;    Values = &Data.BounceAmount.EffectValues; }
    else if (UpgradeName == "FrenzyChance") { Level = Levels->FrenzyChanceLevel;    Values = &Data.FrenzyChance.EffectValues; }

    if (!Values || Values->Num() == 0) return 0.f;
    if (Values->IsValidIndex(Level)) return (*Values)[Level];
    return Values->Last();
}

bool UAbilityManagerComponent::UpgradePlayerStat(const FString& StatName)
{
    if (!PlayerRef) return false;

    FPlayerUpgrade* Target = nullptr;

    if (StatName == "AttackFlat") Target = &AttackFlat;
    else if (StatName == "MaxHPFlat") Target = &MaxHPFlat;
    else if (StatName == "AttackPercent") Target = &AttackPercent;
    else if (StatName == "HPPercent") Target = &HPPercent;

    if (!Target) return false;

    const float Cost = Target->GetNextCost();
    if (PlayerRef->CurrentGold < Cost)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Upgrade] Pas assez d'or pour %s (%.0f requis)"), *StatName, Cost);
        return false;
    }

    // spend gold et upgrade
    PlayerRef->AddGold(-Cost);
    Target->Level++;

    UE_LOG(LogTemp, Log, TEXT("[Upgrade] %s -> Lvl %d | Nouveau cout = %.0f"),
        *StatName, Target->Level, Target->GetNextCost());

    if (StatName == "AttackFlat")
    {
        PlayerRef->Attack = Target->GetCurrentValue();
    }
    else if (StatName == "MaxHPFlat")
    {
        PlayerRef->MaxHP = Target->GetCurrentValue();
        PlayerRef->CurrentHP = FMath::Clamp(PlayerRef->CurrentHP, 0.f, PlayerRef->MaxHP);

        // update du widget
        if (PlayerRef->GameplayWidgetRef)
            PlayerRef->GameplayWidgetRef->UpdateHealth(PlayerRef->CurrentHP, PlayerRef->MaxHP);
    }
    else if (StatName == "AttackPercent")
    {
        // +1% par niveau 
        PlayerRef->AttackMultiplier = 1.0f + (Target->Level * 0.01f);
    }
    else if (StatName == "HPPercent")
    {
        PlayerRef->HealthMultiplier = 1.0f + (Target->Level * 0.01f);

        const float FlatHP = MaxHPFlat.GetCurrentValue();
        const float NewMax = FlatHP * PlayerRef->HealthMultiplier;

        const float Ratio = (PlayerRef->MaxHP > 0.f) ? (PlayerRef->CurrentHP / PlayerRef->MaxHP) : 1.f;
        PlayerRef->MaxHP = NewMax;
        PlayerRef->CurrentHP = FMath::Clamp(NewMax * Ratio, 0.f, NewMax);

        if (PlayerRef->GameplayWidgetRef)
            PlayerRef->GameplayWidgetRef->UpdateHealth(PlayerRef->CurrentHP, PlayerRef->MaxHP);
    }

    return true;
}
