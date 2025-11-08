// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityEffectData.generated.h"

class UAbilityData;
class ACustomCharacter;
class ABaseProjectile;

// Moment ou l'effet se trigger
UENUM(BlueprintType)
enum class EEffectTriggerPhase : uint8
{
    OnCast,
    OnHit,
    OnExpire
};

// Les upgrades sont des attributs qui modifient le behavior, donc on fait des structs ici
// On fait coexister deux systèmes : effect sans upgrades, ca lit directement la valeur input
// Effect avec des upgrades, la le code va lire ces valeurs là, en regardant le lvl de l'upgrade etc

USTRUCT(BlueprintType)
struct FUpgradeStat
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> EffectValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<float> UpgradeCosts;
};

// Les upgrades possibles pour une ability
USTRUCT(BlueprintType)
struct FAbilityUpgradeSet
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FUpgradeStat Damage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FUpgradeStat Cooldown;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FUpgradeStat MultishotChance;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FUpgradeStat MultishotAmount;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FUpgradeStat BounceChance;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FUpgradeStat BounceAmount;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FUpgradeStat FrenzyChance;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FUpgradeStat RetriggerChance;

};

// Context quand on applique un effet
USTRUCT(BlueprintType)
struct FAbilityEffectContext
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite) ACustomCharacter* Source = nullptr;
    UPROPERTY(BlueprintReadWrite) ACustomCharacter* Target = nullptr;
    UPROPERTY(BlueprintReadWrite) const UAbilityData* Ability = nullptr;
    UPROPERTY(BlueprintReadWrite) ABaseProjectile* Projectile = nullptr;
};

// Base de tous les effets
UCLASS(BlueprintType)
class IDLEABILITY_API UAbilityEffectData : public UDataAsset
{
    GENERATED_BODY()

public:
    
    //trigger when ?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    EEffectTriggerPhase TriggerPhase = EEffectTriggerPhase::OnCast;


    // over time
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Over Time")
    float Duration = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Over Time")
    float TickInterval = 1.f;

    // Dans le cas d'un overtime : si dès le call ca trigger ou si ca attend
    // Exemple, durée de 2 sec, tick tous les 0.5 sec. Ca fait cb de tick ? 00, 0.5, 1, 1.5, 2 ? D'ou la clarification
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Over Time")
    bool bTriggerOnApply = true;

    // Ultra important
    // Un effet peut avoir des sous effets qu'ils appliquent a leur cible, comme un projectile qui on hit a freeze + damage + bounce
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    TArray<UAbilityEffectData*> SubEffects;

    // La maniere dont l'effet s'applique, ce qu'il fait en gros, ca depend du type d'effet, un freeze va freeze duh, un dmg va damage.
    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const { return true; }
};
