// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "DragEffectData.generated.h"

// C aurait pu être un pulse effect mais le rendu comme ca est sympa donc j'aime
UCLASS(BlueprintType)
class IDLEABILITY_API UDragEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // Force d'aspiration (distance deplacée)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drag")
    float Strength = 1500.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drag")
    float MaxRange = 500.f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
