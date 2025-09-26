// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityEffectData.generated.h"

class UAbilityData;
class ACustomCharacter;
class ABaseProjectile;

// Moment o� l�effet se d�clenche
UENUM(BlueprintType)
enum class EEffectTriggerPhase : uint8
{
    OnCast,
    OnHit,
    OnExpire,
};

// Contexte d�application
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
    // Quand l�effet doit s�appliquer
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    EEffectTriggerPhase TriggerPhase = EEffectTriggerPhase::OnCast;

    // Effets enfants (modularit�)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    TArray<UAbilityEffectData*> SubEffects;

    // Point d�extension : chaque sous-classe (Damage, Dot, etc.) va override �a
    virtual void ApplyEffect(const FAbilityEffectContext& Context) const {}
};
