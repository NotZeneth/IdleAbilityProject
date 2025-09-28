// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "AOEEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API UAOEEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // rayon de l’AOE (centré sur Target si OnHit, sur Source si OnCast)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE")
    float Radius = 200.f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
