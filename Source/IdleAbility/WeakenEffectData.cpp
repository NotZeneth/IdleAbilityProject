// Fill out your copyright notice in the Description page of Project Settings.


#include "WeakenEffectData.h"
#include "CustomCharacter.h"

bool UWeakenEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Target)
        return false;

    ACustomCharacter* Target = Context.Target;

    Target->DamageTakenBonus += DebuffMagnitude;

    return true; // Manager gère la durée et l'expiration
}
