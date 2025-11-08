// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "RepeatedEffectData.generated.h"

// On repete un effet, en se basant sur la duration et le tick interv

UCLASS(BlueprintType)
class IDLEABILITY_API URepeatedEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Repeat")
    UAbilityEffectData* EffectToRepeat = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Repeat")
    bool bIgnoreTargetDeath = false;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};