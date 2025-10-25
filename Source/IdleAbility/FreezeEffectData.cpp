// Fill out your copyright notice in the Description page of Project Settings.


#include "FreezeEffectData.h"
#include "CustomCharacter.h"

bool UFreezeEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Target)
        return false;

    Context.Target->isFrozen = true;

    return true;
}
