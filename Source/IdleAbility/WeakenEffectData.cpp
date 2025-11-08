// Fill out your copyright notice in the Description page of Project Settings.


#include "WeakenEffectData.h"
#include "CustomCharacter.h"
#include "EnemyCharacter.h"

bool UWeakenEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Target)
        return false;

    // Augmente les dégâts subis
    Context.Target->DamageTakenBonus += DebuffMagnitude;

    // Affiche le plane debuff
    if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Context.Target))
    {
        if (Enemy->WeakenedPlane)
        {
            Enemy->WeakenedPlane->SetHiddenInGame(false);
        }
    }

    return true;
}