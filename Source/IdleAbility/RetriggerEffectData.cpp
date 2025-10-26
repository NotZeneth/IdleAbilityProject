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

    // On cherche la spec active correspondante dans le Manager
    if (TArray<FAbilityEffectSpec>* Specs = Manager->ActiveEffects.Find(Context.Target))
    {
        for (const FAbilityEffectSpec& Spec : *Specs)
        {
            if (Spec.EffectData == this)
            {
                // --- Calcul du nombre de ticks écoulés ---
                const float Elapsed = Duration - Spec.TimeRemaining;
                int32 TicksPassed = FMath::FloorToInt(Elapsed / TickInterval);

                // Corrige le décalage si l'effet n'a pas trigger à l'application
                if (!bTriggerOnApply && TicksPassed > 0)
                {
                    TicksPassed -= 1;
                }

                // Vérifie la limite max
                if (TicksPassed >= MaxTriggers)
                {
                    UE_LOG(LogTemp, Warning,
                        TEXT("[Retrigger] Max atteint (%d/%d) pour %s"),
                        TicksPassed, MaxTriggers, *GetName());
                    return false;
                }

                // Roll de chance
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
                    UE_LOG(LogTemp, Warning,
                        TEXT("[Retrigger] Tick %d -> fail (%.2f > %.2f)"),
                        TicksPassed, Roll, RetriggerChance);

                     return false;
                }

                break; // on sort une fois trouvé
            }
        }
    }

    return true;
}
