// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "UnWeakenEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API UUnWeakenEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;


    // Valeur du debuff : 0.2 = +20% dégâts subis
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weaken")
    float DebuffMagnitude = 0.2f;
};
