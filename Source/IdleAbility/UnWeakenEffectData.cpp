// Fill out your copyright notice in the Description page of Project Settings.


#include "UnWeakenEffectData.h"
#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"
#include "WeakenEffectData.h"

bool UUnWeakenEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Target)
        return false;

    ACustomCharacter* Target = Context.Target;

    UAbilityManagerComponent* Manager =
        Context.Source ? Context.Source->FindComponentByClass<UAbilityManagerComponent>() : nullptr;

    if (!Manager)
        return false;

    Target->DamageTakenBonus -= DebuffMagnitude;

    return true;
}
