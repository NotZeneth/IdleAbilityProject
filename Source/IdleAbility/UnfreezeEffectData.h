// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "UnfreezeEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API UUnfreezeEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
