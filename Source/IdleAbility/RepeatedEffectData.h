// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "RepeatedEffectData.generated.h"

/**
 * Répète un sous-effet à intervalle régulier,
 * en s'appuyant sur le système persistant (ActiveEffects) via Duration/TickInterval.
 * -> Traçable, purgeable, et unifié.
 *
 * Usage :
 *  - EffectToRepeat : l'effet à exécuter à chaque tick (Damage, SpawnProjectile, etc.)
 *  - TickInterval   : intervalle entre exécutions (hérité de UAbilityEffectData)
 *  - Duration       : durée totale (hérité de UAbilityEffectData)
 *  - (Optionnel) RepeatCount : pour l'édition : régler aussi Duration = RepeatCount * TickInterval
 */
UCLASS(BlueprintType)
class IDLEABILITY_API URepeatedEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // L'effet appelé à chaque tick
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Repeat")
    UAbilityEffectData* EffectToRepeat = nullptr;

    // Si vrai, on continue d'exécuter même si la cible meurt (utile pour projectiles "fire forward")
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Repeat")
    bool bIgnoreTargetDeath = false;


    // Important : on utilise Duration/TickInterval hérités pour la persistance/tempo.
    // ApplyEffect sera appelé :
    //  - une fois "OnApply" (lors de l'ajout)
    //  - puis à chaque Tick (TickInterval) tant que Duration > 0
    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};