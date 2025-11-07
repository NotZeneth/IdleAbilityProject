// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "AreaPulseEffectData.generated.h"

/**
 * À chaque tick, applique InnerEffect à tous les ennemis dans un rayon autour du projectile.
 * - Duration / TickInterval : gérés par UAbilityEffectData (persistance via AbilityManager)
 * - InnerEffect doit être "instantané" (Duration=0), typiquement un Damage effect.
 */
UCLASS(BlueprintType)
class IDLEABILITY_API UAreaPulseEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // Effet instantané appliqué à chaque ennemi dans la zone à chaque tick
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulse")
    UAbilityEffectData* InnerEffect = nullptr;

    // Rayon de la zone
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulse", meta = (ClampMin = "0"))
    float Radius = 300.f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
