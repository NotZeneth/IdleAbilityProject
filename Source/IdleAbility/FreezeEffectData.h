#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "FreezeEffectData.generated.h"

UCLASS(BlueprintType)
class IDLEABILITY_API UFreezeEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
