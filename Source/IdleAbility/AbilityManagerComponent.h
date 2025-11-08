// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityData.h"
#include "AbilityEffectSpec.h"
#include "AbilityEffectData.h"
#include "AbilityManagerComponent.generated.h"

USTRUCT(BlueprintType)
struct FPlayerUpgrade
{
    GENERATED_BODY();

    // Nom (facultatif, juste pratique pour debug)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    // Niveau actuel
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;

    // Valeur de base (ex : 10 d’attaque ou 100 PV)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseValue = 0.f;

    // Croissance exponentielle (ex : 1.15 = +15% par niveau)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ValueGrowth = 1.15f;

    // Coût de base de la première upgrade
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseCost = 100.f;

    // Croissance du coût (ex : 1.25 = +25% par niveau)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CostGrowth = 1.25f;

    // Donne la valeur actuelle selon le niveau
    float GetCurrentValue() const
    {
        return BaseValue * FMath::Pow(ValueGrowth, Level);
    }

    // Donne le coût de la prochaine upgrade
    float GetNextCost() const
    {
        return BaseCost * FMath::Pow(CostGrowth, Level);
    }
};



// Spécifie une ability équipée par le joueur
USTRUCT(BlueprintType)
struct FAbilitySpec
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowedClasses = "AbilityData"))
    UAbilityData* Ability = nullptr;

    float CooldownEndTime = 0.f;
    float CooldownScalar = 1.f;

    // c'est le widget qui va dire si on le passe en auto, c'est pas ideal mais ca fonctionne
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    bool isAutoCast = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    bool bUnlocked = false;
};

// Pour les upgrades, relou i know mais faut garder leur lvl traqué hein
USTRUCT(BlueprintType)
struct FUpgradeLevels
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DamageLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CooldownLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MultishotChanceLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MultishotAmountLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BounceChanceLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BounceAmountLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 FrenzyChanceLevel = 0;
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

    class APlayerCharacter* PlayerRef;

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
    TMap<const UAbilityEffectData*, int32> RetriggerCounts;

    UPROPERTY()
    TArray<ACustomCharacter*> PendingRemovals;

    UPROPERTY()
    class AWaveGameMode* GameModeRef = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void ResetAllEffectsAndCooldowns();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Upgrades")
    TMap<UAbilityData*, FUpgradeLevels> UpgradeLevelsByAbility;

    // Get current value of an upgrade (from data)
    UFUNCTION(BlueprintCallable, Category = "Upgrades")
    float GetUpgradeValue(const UAbilityData* Ability, const FString& UpgradeName) const;

    // Upgrade a specific stat for an ability
    UFUNCTION(BlueprintCallable, Category = "Upgrades")
    void UpgradeAbility(UAbilityData* Ability, FString UpgradeName);

    // --- Upgrades du joueur ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Player")
    FPlayerUpgrade AttackFlat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Player")
    FPlayerUpgrade MaxHPFlat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Player")
    FPlayerUpgrade AttackPercent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Player")
    FPlayerUpgrade HPPercent;

    // Upgrade d'une stat (flat ou %)
    UFUNCTION(BlueprintCallable, Category = "Upgrades|Player")
    bool UpgradePlayerStat(const FString& StatName);



};