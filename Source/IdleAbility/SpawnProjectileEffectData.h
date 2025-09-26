// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "SpawnProjectileEffectData.generated.h"

class ABaseProjectile;

UCLASS(BlueprintType)
class IDLEABILITY_API USpawnProjectileEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    TSubclassOf<ABaseProjectile> ProjectileClass;

    UPROPERTY()
    TArray<UAbilityEffectData*> EffectsOnHit;

    virtual void ApplyEffect(const FAbilityEffectContext& Context) const override;

    
};
