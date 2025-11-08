// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "AbilityEffectSpec.generated.h"

// Effect actif appliqué sur une cible, un truc stocked qui peut survivre, typiquement utile pour freeze, dot etc
USTRUCT(BlueprintType)
struct FAbilityEffectSpec
{
    GENERATED_BODY()

    UPROPERTY()
    const UAbilityEffectData* EffectData = nullptr;

    //source, cible, projectile...
    UPROPERTY()
    FAbilityEffectContext Context;

    float TimeRemaining = 0.f;

    // Pour les trucs qui custom tick : c'est pas le tick d'unreal, mais tick comme une dot qui tick quoi
    float TimeSinceLastTick = 0.f;

    FAbilityEffectSpec() = default;

    FAbilityEffectSpec(const UAbilityEffectData* InData, const FAbilityEffectContext& InContext)
        : EffectData(InData), Context(InContext)
    {
    }

    // Pour gagner du temps ca
    bool operator==(const FAbilityEffectSpec& Other) const
    {
        return EffectData == Other.EffectData && Context.Target == Other.Context.Target;
    }
};
