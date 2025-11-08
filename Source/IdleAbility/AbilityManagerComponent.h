// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityData.h"
#include "AbilityEffectData.h"
#include "AbilityEffectSpec.h"
#include "AbilityManagerComponent.generated.h"

class ACustomCharacter;
class AEnemyCharacter;
class AWaveGameMode;
class APlayerCharacter;

// Ce qu'est une upgrade de stat pour le player
USTRUCT(BlueprintType)
struct FPlayerUpgrade
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseValue = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ValueGrowth = 1.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BaseCost = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CostGrowth = 1.25f;

    float GetCurrentValue() const { return BaseValue * FMath::Pow(ValueGrowth, Level); }
    float GetNextCost() const { return BaseCost * FMath::Pow(CostGrowth, Level); }
};

// Une ability equiped par le joueur
USTRUCT(BlueprintType)
struct FAbilitySpec
{
    GENERATED_BODY();

    // Le data asset de l'ability 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowedClasses = "AbilityData"))
    UAbilityData* Ability = nullptr;

    float CooldownEndTime = 0.f;

    float CooldownScalar = 1.f;

    // On edit pas la valeur du data asset au runtime, donc on utilise ca indeed
    // Ca prend au debut celle du DA, et ensuite on l'update if needed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    bool isAutoCast = false;

    // Same
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    bool bUnlocked = false;
};

USTRUCT(BlueprintType)
struct FUpgradeLevels
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 DamageLevel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 CooldownLevel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 MultishotChanceLevel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 MultishotAmountLevel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 BounceChanceLevel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 BounceAmountLevel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 FrenzyChanceLevel = 0;
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

    UPROPERTY()
    APlayerCharacter* PlayerRef = nullptr;

    // Liste des ability
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TArray<FAbilitySpec> EquippedAbilities;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void TryActivateAbility(int32 AbilityIndex);

    bool IsAbilityReady(const FAbilitySpec& Spec) const;

    void ExecuteAbility(const FAbilitySpec& Spec);

    void FindTargets(const UAbilityData* Ability, ACustomCharacter* Caster, TArray<ACustomCharacter*>& OutTargets) const;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void GetEnemiesInRange(const ACustomCharacter* Origin, float Range, TArray<ACustomCharacter*>& Out) const;

    //map des effets actifs sur chaque cible : une dot, un freeze etc
    TMap<ACustomCharacter*, TArray<FAbilityEffectSpec>> ActiveEffects;

    UFUNCTION()
    void OnEnemyKilled(AEnemyCharacter* DeadEnemy);

    void ApplyEffectToTarget(const UAbilityEffectData* EffectData, const FAbilityEffectContext& Context);

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void ResetAllEffectsAndCooldowns();

    UPROPERTY()
    TArray<ACustomCharacter*> PendingRemovals;

    UPROPERTY()
    AWaveGameMode* GameModeRef = nullptr;

    UPROPERTY()
    TMap<const UAbilityData*, float> LastFrenzyTimes;

    UPROPERTY()
    TMap<const UAbilityEffectData*, int32> RetriggerCounts;

    // Upgrades d'abilities

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Upgrades")
    TMap<UAbilityData*, FUpgradeLevels> UpgradeLevelsByAbility;

    UFUNCTION(BlueprintCallable, Category = "Upgrades")
    float GetUpgradeValue(const UAbilityData* Ability, const FString& UpgradeName) const;

    UFUNCTION(BlueprintCallable, Category = "Upgrades")
    void UpgradeAbility(UAbilityData* Ability, FString UpgradeName);

    // Upgrade des stats du joueur

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Player")
    FPlayerUpgrade AttackFlat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Player")
    FPlayerUpgrade MaxHPFlat;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Player")
    FPlayerUpgrade AttackPercent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Upgrades|Player")
    FPlayerUpgrade HPPercent;

    UFUNCTION(BlueprintCallable, Category = "Upgrades|Player")
    bool UpgradePlayerStat(const FString& StatName);
};
