// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityEffectData.generated.h"

class UAbilityData;
class ACustomCharacter;
class ABaseProjectile;

// When the effect triggers
UENUM(BlueprintType)
enum class EEffectTriggerPhase : uint8
{
    OnCast,
    OnHit,
    OnExpire,
};

USTRUCT(BlueprintType)
struct FUpgradeStat
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> EffectValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> UpgradeCosts;
};

USTRUCT(BlueprintType)
struct FAbilityUpgradeSet
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUpgradeStat Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUpgradeStat Cooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUpgradeStat MultishotChance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUpgradeStat MultishotAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUpgradeStat BounceChance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUpgradeStat BounceAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUpgradeStat FrenzyChance;
};

// Effect application context
USTRUCT(BlueprintType)
struct FAbilityEffectContext
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite)
    ACustomCharacter* Source = nullptr;

    UPROPERTY(BlueprintReadWrite)
    ACustomCharacter* Target = nullptr;

    UPROPERTY(BlueprintReadWrite)
    const UAbilityData* Ability = nullptr;

    UPROPERTY(BlueprintReadWrite)
    ABaseProjectile* Projectile = nullptr;
};

// Base effect asset
UCLASS(BlueprintType)
class IDLEABILITY_API UAbilityEffectData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    EEffectTriggerPhase TriggerPhase = EEffectTriggerPhase::OnCast;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Over Time")
    float Duration = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Over Time")
    float TickInterval = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Over Time")
    bool bTriggerOnApply = true;

    // Optional chained sub-effects
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    TArray<UAbilityEffectData*> SubEffects;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const { return true; }
};
