// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbilityEffect.generated.h"

UENUM(BlueprintType)
enum class EEffectTriggerPhase : uint8
{
    OnCast,
    OnHit,
    OnExpire,
};

class ACustomCharacter;
class UAbilityData;

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class IDLEABILITY_API UAbilityEffect : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ability")
    void ApplyEffect(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData);
    virtual void ApplyEffect_Implementation(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData) {}

    // À quel moment cet effet se déclenche (OnCast, OnHit, OnExpire…)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TriggerPhase")
    EEffectTriggerPhase TriggerPhase = EEffectTriggerPhase::OnCast;


    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "TriggerPhase")
    TArray<UAbilityEffect*> SubEffects;
};
