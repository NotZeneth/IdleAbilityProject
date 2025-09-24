// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "DotEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class IDLEABILITY_API UDotEffect : public UAbilityEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoT", meta = (ClampMin = "0.1"))
    float TickInterval = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DoT", meta = (ClampMin = "0.1"))
    float Duration = 3.f;

    virtual void ApplyEffect_Implementation(const FAbilityEffectContext& Context) override;

private:
    void ApplyTick(const FAbilityEffectContext Context);
};


