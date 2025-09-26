// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "AbilityEffectSpec.generated.h"

USTRUCT(BlueprintType)
struct FAbilityEffectSpec
{
    GENERATED_BODY();

    // Référence vers la data
    UPROPERTY()
    const UAbilityEffectData* EffectData = nullptr;

    // Contexte d’application
    FAbilityEffectContext Context;

    // Runtime : exemple durée restante (modifiable par les sous-classes)
    float TimeRemaining = 0.f;

    FAbilityEffectSpec() {}
    FAbilityEffectSpec(const UAbilityEffectData* InData, const FAbilityEffectContext& InContext)
        : EffectData(InData), Context(InContext)
    {
    }
};
