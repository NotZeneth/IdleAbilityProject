// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilityEffect.generated.h"

class ACustomCharacter;
class UAbilityData;
class ABaseProjectile;

UENUM(BlueprintType)
enum class EEffectTriggerPhase : uint8
{
    OnCast,
    OnHit,
    OnExpire,
};


USTRUCT(BlueprintType)
struct FAbilityEffectContext // C'est juste des refs donc ca coute r, et ca permet d'avoir toutes les infos
                             // Ca permet une modularite infinie, on peut dupliquer, filtrer modifier, faire ce qu'on veut via ces infos !
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    ACustomCharacter* Source = nullptr;

    UPROPERTY(BlueprintReadWrite)
    ACustomCharacter* Target = nullptr;

    UPROPERTY(BlueprintReadWrite)
    const UAbilityData* AbilityData = nullptr;

    UPROPERTY(BlueprintReadWrite)
    ABaseProjectile* Projectile = nullptr; // Tout object instancie est un projectile : meme un mur est un projectile avec une speed de 0 en soit
};

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class IDLEABILITY_API UAbilityEffect : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
    void ApplyEffect(const FAbilityEffectContext& Context);
    virtual void ApplyEffect_Implementation(const FAbilityEffectContext& Context) {}

    // À quel moment cet effet se déclenche (OnCast, OnHit, OnExpire…)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerPhase")
    EEffectTriggerPhase TriggerPhase = EEffectTriggerPhase::OnCast;

    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "TriggerPhase")
    TArray<UAbilityEffect*> SubEffects;
};
