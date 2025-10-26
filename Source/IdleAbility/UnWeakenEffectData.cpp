// Fill out your copyright notice in the Description page of Project Settings.


#include "UnWeakenEffectData.h"
#include "CustomCharacter.h"
#include "EnemyCharacter.h"

bool UUnWeakenEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Target)
        return false;

    // On retire le malus de dégâts reçus
    Context.Target->DamageTakenBonus -= DebuffMagnitude;
    if (Context.Target->DamageTakenBonus < 0.f)
    {
        Context.Target->DamageTakenBonus = 0.f;
    }

    // Si plus d'affaiblissement actif, cacher le visuel
    if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Context.Target))
    {
        if (Enemy->WeakenedPlane && Context.Target->DamageTakenBonus <= 0.f)
        {
            Enemy->WeakenedPlane->SetHiddenInGame(true);
        }
    }

    return true;
}