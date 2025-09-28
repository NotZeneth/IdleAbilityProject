// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "DamageEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API UDamageEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // Multiplie l’attaque du joueur
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    float DamageMultiplier = 1.0f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};