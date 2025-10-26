// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "DragEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API UDragEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // Force d'aspiration par tick (distance déplacée)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drag")
    float Strength = 1500.f;

    // Distance maximale d'influence de la tornade
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drag")
    float MaxRange = 500.f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
