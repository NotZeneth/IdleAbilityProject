// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "UnFrenzyEffectData.generated.h"

UCLASS()
class IDLEABILITY_API UUnFrenzyEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // doit être exactement l’inverse du FrenzyScalar
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frenzy")
    float RevertScalar = 5.0f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
