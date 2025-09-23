// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilityData.h"
#include "AbilityEffect.generated.h"

class ACustomCharacter;

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class IDLEABILITY_API UAbilityEffect : public UObject
{
    GENERATED_BODY()

public:
    // Applique l'effet à une cible
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
    void ApplyEffect(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData);
    virtual void ApplyEffect_Implementation(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData) {}
};
