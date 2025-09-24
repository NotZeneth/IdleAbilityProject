// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "BounceEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class IDLEABILITY_API UBounceEffect : public UAbilityEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bounce")
    float ChanceMultiplier = 1.f;

    virtual void ApplyEffect_Implementation(const FAbilityEffectContext& Context) override;
};
