// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "BounceEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API UBounceEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // Multiplie la stat BounceChance du joueur
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bounce")
    float ChanceMultiplier = 1.0f;

    // Si Ability->Range == 0, fallback sur cette valeur
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bounce")
    float BounceRangeOverride = 1000.f;

    virtual void ApplyEffect(const FAbilityEffectContext& Context) const override;
};
