// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityData.h"
#include "AbilityEffectSpec.h"
#include "AbilityManagerComponent.generated.h"

// Spécifie une ability équipée par le joueur
USTRUCT(BlueprintType)
struct FAbilitySpec
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowedClasses = "AbilityData"))
    UAbilityData* Ability = nullptr;

    float CooldownEndTime = 0.f;
    float CooldownScalar = 1.f;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class IDLEABILITY_API UAbilityManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAbilityManagerComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Liste d'abilities équipées
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TArray<FAbilitySpec> EquippedAbilities;

    // Activer une ability par index
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void TryActivateAbility(int32 AbilityIndex);

    // Vérifie cooldown
    bool IsAbilityReady(const FAbilitySpec& Spec) const;

    void ExecuteAbility(const FAbilitySpec& Spec);

    void FindTargets(const UAbilityData* Ability, class ACustomCharacter* Caster, TArray<ACustomCharacter*>& OutTargets) const;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void GetEnemiesInRange(const class ACustomCharacter* Origin, float Range, TArray<class ACustomCharacter*>& Out) const;

    // ---- Effets persistants (DOTs, Frenzy, etc.) ----
    TMap<ACustomCharacter*, TArray<FAbilityEffectSpec>> ActiveEffects;

    UFUNCTION()
    void OnEnemyKilled(class AEnemyCharacter* DeadEnemy);

    // Pour appliquer un effet et gérer les persistants
    void ApplyEffectToTarget(const UAbilityEffectData* EffectData, const FAbilityEffectContext& Context);

    // Pour frenzy
    UPROPERTY() 
    TMap<const UAbilityData*, float> LastFrenzyTimes;

    UPROPERTY()
    TArray<ACustomCharacter*> PendingRemovals;

};