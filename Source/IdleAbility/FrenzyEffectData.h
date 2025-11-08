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
    // Chance locale (0..1) si pas d’upgrade
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frenzy")
    float TriggerChance = 1.0f;

    // Multiplicateur appliqué au cooldown scalar lors du proc
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frenzy")
    float FrenzyScalar = 0.2f;

    // Délai entre deux procs
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frenzy")
    float InternalCooldown = 5.0f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
