 // Fill out your copyright notice in the Description page of Project Settings.

#include "RepeatedEffectData.h"
#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"
#include "Engine/World.h"

bool URepeatedEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Source || !EffectToRepeat)
    {
        return true;
    }

    if (Context.Target && !bIgnoreTargetDeath && !Context.Target->IsAlive())
    {
        return true;
    }

    UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager)
    {
        EffectToRepeat->ApplyEffect(Context);
        return true;
    }

    Manager->ApplyEffectToTarget(EffectToRepeat, Context);
    return true;
}
