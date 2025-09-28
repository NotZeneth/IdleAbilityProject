// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "FrenzyEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API UFrenzyEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frenzy")
    float ChanceMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frenzy")
    float FrenzyScalar = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frenzy")
    float InternalCooldown = 5.0f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
