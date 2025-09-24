#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CustomCharacter.h" // pour l'enum dmg type
#include "AbilityData.generated.h"

class UAbilityEffect;

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
    float Range = 2000.f; // rayon de recherche

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (EditCondition = "Targeting==EAbilityTargeting::RandomEnemies", ClampMin = "1"))
    int32 TargetCount = 1;

    // ---- Effets (data-driven) ----
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Effects")
    TArray<UAbilityEffect*> Effects; // ex: Damage, Heal, Buff, etc.
};
