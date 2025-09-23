// AbilityData.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityData.generated.h"

// Type de d�clenchement de l'ability
UENUM(BlueprintType)
enum class EAbilityTriggerType : uint8
{
    Manual     UMETA(DisplayName = "Manual"),    // Le joueur clique sur un bouton
    Auto       UMETA(DisplayName = "Auto"),      // Lanc� automatiquement
    OnEvent    UMETA(DisplayName = "On Event")   // Se d�clenche via un �v�nement (ex: projectile hit)
};

UCLASS(BlueprintType)
class IDLEABILITY_API UAbilityData : public UDataAsset
{
    GENERATED_BODY()

public:

    // Nom affich�
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    FText AbilityName;

    // Ic�ne (UI)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    UTexture2D* Icon;

    // Cooldown en secondes
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float Cooldown;

    // Dur�e (0 = instantan�)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float Duration;

    // Type de d�clenchement
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    EAbilityTriggerType TriggerType;

    // Valeurs principales (ex: d�g�ts, heal, slow %, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float PrimaryValue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
    float SecondaryValue;
};
