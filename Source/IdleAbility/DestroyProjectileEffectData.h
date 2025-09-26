// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "DestroyProjectileEffectData.generated.h"

UCLASS()
class IDLEABILITY_API UDestroyProjectileEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    virtual void ApplyEffect(const FAbilityEffectContext& Context) const override;
};
