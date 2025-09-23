// AbilityData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityData.generated.h"

// Type de déclenchement de l'ability
UENUM(BlueprintType)
enum class EAbilityTriggerType : uint8
{
    Manual     UMETA(DisplayName = "Manual"),    // Le joueur clique sur un bouton
    Auto       UMETA(DisplayName = "Auto"),      // Lancé automatiquement
    OnEvent    UMETA(DisplayName = "On Event")   // Se déclenche via un évènement (ex: projectile hit)
};

UCLASS(BlueprintType)
class IDLEABILITY_API UAbilityData : public UDataAsset
{
    GENERATED_BODY()

public:

    // Nom affiché
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    FText AbilityName;

    // Icône (UI)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    UTexture2D* Icon;

    // Cooldown en secondes
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float Cooldown;

    // Durée (0 = instantané)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float Duration;

    // Type de déclenchement
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    EAbilityTriggerType TriggerType;

    // Valeurs principales (ex: dégâts, heal, slow %, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float PrimaryValue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float SecondaryValue;
};
