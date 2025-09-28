// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "AbilityEffectSpec.generated.h"

USTRUCT(BlueprintType)
struct FAbilityEffectSpec
{
    GENERATED_BODY()

    UPROPERTY()
    const UAbilityEffectData* EffectData = nullptr;

    UPROPERTY()
    FAbilityEffectContext Context;

    // Durée restante (0 = instantané)
    float TimeRemaining = 0.f;

    // Pour accumuler le temps entre ticks (si durée > 0)
    float TimeSinceLastTick = 0.f;

    FAbilityEffectSpec() {}
    FAbilityEffectSpec(const UAbilityEffectData* InData, const FAbilityEffectContext& InContext)
        : EffectData(InData), Context(InContext) {
    }

    bool operator==(const FAbilityEffectSpec& Other) const
    {
        return EffectData == Other.EffectData && Context.Target == Other.Context.Target;
    }
};
