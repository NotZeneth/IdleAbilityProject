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
    // QUand on utilise pas d'upgrade ca
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bounce")
    float BounceChance = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bounce")
    int BounceCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bounce")
    float BounceRangeOverride = 1000.f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
