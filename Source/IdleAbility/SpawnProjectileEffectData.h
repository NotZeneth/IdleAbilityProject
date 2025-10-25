// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "SpawnProjectileEffectData.generated.h"

class ABaseProjectile;

USTRUCT(BlueprintType)
struct FWeightedProjectile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    TSubclassOf<ABaseProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    float Weight = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    TArray<UAbilityEffectData*> ExtraSubEffects;
};

UCLASS(BlueprintType)
class IDLEABILITY_API USpawnProjectileEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
    TArray<FWeightedProjectile> ProjectileOptions;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};