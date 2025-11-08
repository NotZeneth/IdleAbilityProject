// Fill out your copyright notice in the Description page of Project Settings.


#include "RetriggerEffectData.h"
#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"

bool URetriggerEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!EffectToRepeat || !Context.Source)
        return false;

    UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager)
        return false;

    if (TArray<FAbilityEffectSpec>* Specs = Manager->ActiveEffects.Find(Context.Target))
    {
        for (FAbilityEffectSpec& Spec : *Specs)
        {
            if (Spec.EffectData == this)
            {
                const float Elapsed = Duration - Spec.TimeRemaining;
                int32 TicksPassed = FMath::FloorToInt(Elapsed / TickInterval);

                if (!bTriggerOnApply && TicksPassed > 0)
                {
                    TicksPassed -= 1;
                }

                if (TicksPassed >= MaxTriggers)
                {
                    Spec.TimeRemaining = -99999.f;
                    return false;
                }

                float Chance = RetriggerChance;
                if (Manager && Context.Ability)
                {
                    const float UpChance = Manager->GetUpgradeValue(Context.Ability, TEXT("RetriggerChance"));
                    const FAbilityUpgradeSet& Up = Context.Ability->BaseUpgrades;

                    if (Up.RetriggerChance.EffectValues.Num() > 0)
                        Chance = FMath::Clamp(UpChance, 0.f, 1.f);
                }

                const float Roll = FMath::FRand();
                UE_LOG(LogTemp, Warning, TEXT("[Retrigger] Roll=%.2f | Chance=%.2f"), Roll, Chance);

                if (Roll <= Chance)
                {
                    UE_LOG(LogTemp, Warning,
                        TEXT("[Retrigger] Tick %d -> success (%.2f <= %.2f)"),
                        TicksPassed, Roll, Chance);

                    return URepeatedEffectData::ApplyEffect(Context);
                }

                UE_LOG(LogTemp, Warning,
                    TEXT("[Retrigger] Tick %d -> fail (%.2f > %.2f)"),
                    TicksPassed, Roll, Chance);

                Spec.TimeRemaining = -99.f;
                return false;
            }
        }
    }

    return true;
}
