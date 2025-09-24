// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "BaseProjectile.h"
#include "SpawnProjectileEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class IDLEABILITY_API USpawnProjectileEffect : public UAbilityEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    TSubclassOf<ABaseProjectile> ProjectileClass;

    virtual void ApplyEffect_Implementation(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData) override;
};

