// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "DestroyProjectileEffectData.generated.h"

// Bah un projectile est pas forcement detruit : que ce soit on cast, on expire ou on hit, l'effet est interessant
// Certains utilisent juste le "est detruit apres X sec"
UCLASS()
class IDLEABILITY_API UDestroyProjectileEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
