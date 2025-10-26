// Fill out your copyright notice in the Description page of Project Settings.


#include "RetriggerEffectData.h"
#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"

bool URetriggerEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!EffectToRepeat || !Context.Source)
        return false;

    UAbilityManagerComponent* Manager =
        Context.Source->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager) return false;

    int32& Count = Manager->RetriggerCounts.FindOrAdd(this);

    // Stop si limite atteinte
    if (Count >= MaxTriggers)
    {
        Manager->RetriggerCounts.Remove(this);
        return false;
    }

    const float Roll = FMath::FRand();

    if (Roll <= RetriggerChance)
    {
        Count++;
        return URepeatedEffectData::ApplyEffect(Context);
    }

    // Échec => stop définitif
    Manager->RetriggerCounts.Remove(this);
    return false;
}
