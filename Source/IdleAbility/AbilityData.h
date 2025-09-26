// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CustomCharacter.h" 
#include "AbilityData.generated.h"

class UAbilityEffectData;

UENUM(BlueprintType)
enum class EAbilityTriggerType : uint8
{
    Manual   UMETA(DisplayName = "Manual"),
    Auto     UMETA(DisplayName = "Auto"),
    OnEvent  UMETA(DisplayName = "On Event")
};

UENUM(BlueprintType)
enum class EAbilityTargeting : uint8
{
    Self                UMETA(DisplayName = "Self"),
    SingleNearestEnemy  UMETA(DisplayName = "Single Nearest Enemy"),
    AllEnemiesInRange   UMETA(DisplayName = "All Enemies In Range"),
    RandomEnemies       UMETA(DisplayName = "Random Enemies")
};

UCLASS(BlueprintType)
class IDLEABILITY_API UAbilityData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    FText AbilityName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float Cooldown = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float Duration = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    EAbilityTriggerType TriggerType = EAbilityTriggerType::Manual;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    EDamageType AbilityType = EDamageType::Pure;

    // Deux “tracks” génériques pour upgrades (dégâts, %slow, durée, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float PrimaryValue = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float SecondaryValue = 0.f;

    // ---- Targeting ----
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
    EAbilityTargeting Targeting = EAbilityTargeting::SingleNearestEnemy;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (EditCondition = "Targeting!=EAbilityTargeting::Self"))
    float Range = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (EditCondition = "Targeting==EAbilityTargeting::RandomEnemies", ClampMin = "1"))
    int32 TargetCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TArray<UAbilityEffectData*> Effects;
};
