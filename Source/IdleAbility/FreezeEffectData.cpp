// Fill out your copyright notice in the Description page of Project Settings.


#include "FreezeEffectData.h"
#include "CustomCharacter.h"
#include "EnemyCharacter.h"

bool UFreezeEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Target)
        return false;

    Context.Target->isFrozen = true;

    if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Context.Target))
    {
        if (Enemy->FrozenPlane)
        {
            Enemy->FrozenPlane->SetHiddenInGame(false);
        }
    }

    return true;
}