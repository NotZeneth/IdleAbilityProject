// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "MultishotEffectData.generated.h"

UCLASS()
class IDLEABILITY_API UMultishotEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // Multiplie la chance du joueur (1.0 = pas de changement)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Multishot")
    float ChanceMultiplier = 1.0f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
