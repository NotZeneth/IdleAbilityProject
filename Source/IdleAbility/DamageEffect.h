#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "CustomCharacter.h" // <-- ajoute ceci pour avoir EDamageType

#include "DamageEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew)
class IDLEABILITY_API UDamageEffect : public UAbilityEffect
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
    EDamageType DamageType = EDamageType::Physical;

    virtual void ApplyEffect_Implementation(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData) override;
};
