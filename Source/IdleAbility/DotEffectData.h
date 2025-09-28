// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "DotEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API UDotEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // Multiplie l’attaque du lanceur à chaque tick
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DOT")
    float DamagePerTickMultiplier = 0.5f;

    // Bonus plat par tick (optionnel)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DOT")
    float FlatDamagePerTick = 0.f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
