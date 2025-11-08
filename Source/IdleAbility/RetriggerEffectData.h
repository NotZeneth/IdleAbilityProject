// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RepeatedEffectData.h"
#include "RetriggerEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API URetriggerEffectData : public URepeatedEffectData
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Retrigger",
        meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float RetriggerChance = 1.0f;

    // Nombre max de retriggers
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Retrigger",
        meta = (ClampMin = "1"))
    int32 MaxTriggers = 1;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
