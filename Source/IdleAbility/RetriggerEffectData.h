// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "RetriggerEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API URetriggerEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // L'effet à relancer (souvent un SpawnProjectile, Damage, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Retrigger")
    UAbilityEffectData* EffectToRetrigger = nullptr;

    // Chance de relancer (0 = jamais, 1 = toujours)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Retrigger", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float RetriggerChance = 1.0f;

    // Nombre maximum de retriggers avant arrêt
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Retrigger", meta = (ClampMin = "1"))
    int32 MaxRetriggerCount = 1;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
