// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityEffectData.generated.h"

class UAbilityData;
class ACustomCharacter;
class ABaseProjectile;

// Moment où l’effet se déclenche
UENUM(BlueprintType)
enum class EEffectTriggerPhase : uint8
{
    OnCast,
    OnHit,
    OnExpire,
};

// Contexte d’application
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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    TArray<UAbilityEffectData*> SubEffects;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const { return true; }
};

