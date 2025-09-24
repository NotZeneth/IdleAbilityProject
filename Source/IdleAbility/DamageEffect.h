#pragma once

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffect.h"
#include "DamageEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class IDLEABILITY_API UDamageEffect : public UAbilityEffect
{
    GENERATED_BODY()

public:

    virtual void ApplyEffect_Implementation(const FAbilityEffectContext& Context) override;
};
