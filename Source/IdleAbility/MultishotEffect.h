// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "MultishotEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class IDLEABILITY_API UMultishotEffect : public UAbilityEffect
{
    GENERATED_BODY()

public:
    // % de chance d’activer le multishot (0..1)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multishot")
    float Chance = 1.0f; // pour tester à 100%

    // Mélanger les cibles pour randomiser
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multishot")
    bool bRandomizeTargets = true;

    virtual void ApplyEffect_Implementation(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData) override;
};
