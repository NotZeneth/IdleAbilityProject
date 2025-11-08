// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEffectData.h"
#include "AreaPulseEffectData.generated.h"

// Ca applique l'effet autour du projectile periodiquement
// La dif c'est que le systeme est plutot pensé pour mettre un debuff, mettre un truc timé sur des cibles, pas refaire autour d'un projectile
// D'ou cet effet, pour n'imp quel truc qui doit faire periodiquement un truc autour de lui
// Ironiquement je l'utilise pas pour la tornade, car somehow le resultat actuel, non volontaire, rend mieux que ce que je voulais de base

// Seul le spike trap l'utilise atm

UCLASS(BlueprintType)
class IDLEABILITY_API UAreaPulseEffectData : public UAbilityEffectData
{
    GENERATED_BODY()

public:
    // Effet instantané appliqué à chaque ennemi dans la zone à chaque tick
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulse")
    UAbilityEffectData* InnerEffect = nullptr;

    // Rayon de la zone
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulse", meta = (ClampMin = "0"))
    float Radius = 300.f;

    virtual bool ApplyEffect(const FAbilityEffectContext& Context) const override;
};
