// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "DestroyOnHitEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class IDLEABILITY_API UDestroyOnHitEffect : public UAbilityEffect
{
    GENERATED_BODY()

public:
    UDestroyOnHitEffect()
    {
        TriggerPhase = EEffectTriggerPhase::OnHit;
    }

    virtual void ApplyEffect_Implementation(const FAbilityEffectContext& Context) override;
};

