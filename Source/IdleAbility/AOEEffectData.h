// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "AOEEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API UAOEEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // rayon de l’AOE, 200 c'est deux cubes (quand on compare au background / sol) ca aide a preview
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE")
    float Radius = 200.f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
