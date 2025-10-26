// Fill out your copyright notice in the Description page of Project Settings.


#include "UnfreezeEffectData.h"
#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"
#include "EnemyCharacter.h"
#include "FreezeEffectData.h"
#include "AbilityEffectSpec.h"

bool UUnfreezeEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Target)
    {
        return false;
    }

    ACustomCharacter* Target = Context.Target;

    UAbilityManagerComponent* Manager =
        Context.Source ? Context.Source->FindComponentByClass<UAbilityManagerComponent>() : nullptr;

    if (!Manager)
    {
        return false;
    }

    // Debug: afficher la liste des effets restants
    if (TArray<FAbilityEffectSpec>* Specs = Manager->ActiveEffects.Find(Target))
    {

        for (const FAbilityEffectSpec& Spec : *Specs)
        {
            if (!Spec.EffectData) continue;

            // On ignore le Freeze qui est en train d’expirer
            if (Spec.EffectData->IsA<UFreezeEffectData>() && Spec.TimeRemaining > 0.f)
            {
                return true;
            }
        }

    }

    // Si pas de Freeze restant : unfreeze
    Target->isFrozen = false;

    if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Target))
    {
        if (Enemy->FrozenPlane)
        {
            Enemy->FrozenPlane->SetHiddenInGame(true);
        }
    }

    return true;
}
