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
                    Spec.TimeRemaining = -99999.f; //  fin 
                    return false;
                }

                const float Roll = FMath::FRand();

                if (Roll <= RetriggerChance)
                {
                    UE_LOG(LogTemp, Warning,
                        TEXT("[Retrigger] Tick %d -> success (%.2f <= %.2f)"),
                        TicksPassed, Roll, RetriggerChance);

                    return URepeatedEffectData::ApplyEffect(Context);
                }
                else
                {
                    Spec.TimeRemaining = -99.f; //  fin 
                    return false;
                }
            }
        }
    }

    return true;
}