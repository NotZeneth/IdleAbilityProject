// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityEffectData.h"
#include "CustomCharacter.h"
#include "AbilityData.generated.h"

//On event on utilise pas fnalement, on aurait pu tho
UENUM(BlueprintType)
enum class EAbilityTriggerType : uint8
{
    Manual   UMETA(DisplayName = "Manual"),
    Auto     UMETA(DisplayName = "Auto"),
    OnEvent  UMETA(DisplayName = "On Event")
};

// self on l'utilise pas et on l'a jamais test du coup
UENUM(BlueprintType)
enum class EAbilityTargeting : uint8
{
    Self                UMETA(DisplayName = "Self"),
    SingleNearestEnemy  UMETA(DisplayName = "Single Nearest Enemy"),
    AllEnemiesInRange   UMETA(DisplayName = "All Enemies In Range"),
    RandomEnemies       UMETA(DisplayName = "Random Enemies")
};

// Tout ce que contient une ability

UCLASS(BlueprintType)
class IDLEABILITY_API UAbilityData : public UDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    FText AbilityName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float Cooldown = 1.f;

    // 0 = instant, classique
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float Duration = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    EAbilityTriggerType TriggerType = EAbilityTriggerType::Manual;

    // Au final on a juste mis tout a pure, vu qu'on utilise pas d'armure par flmm de balance mais c'est coded
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    EDamageType AbilityType = EDamageType::Pure;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
    EAbilityTargeting Targeting = EAbilityTargeting::SingleNearestEnemy;

    // Range a laquelle on considere l'enemie comme ciblable par l'ability
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting",
        meta = (EditCondition = "Targeting!=EAbilityTargeting::Self"))
    float Range = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting",
        meta = (EditCondition = "Targeting==EAbilityTargeting::RandomEnemies", ClampMin = "1"))
    int32 TargetCount = 1;



    // Liste des effets
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TArray<UAbilityEffectData*> Effects;

    //liste des upgrades
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrade")
    FAbilityUpgradeSet BaseUpgrades;

    // Si l'ability commence unlocked ou non
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    bool bUnlocked = true;

    // Cout pour unlock
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
    int32 GemCostToUnlock = 1;
};
    
