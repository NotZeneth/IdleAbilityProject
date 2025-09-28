// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomCharacter.h"
#include "PlayerCharacter.generated.h"

class UAbilityManagerComponent;

UCLASS()
class IDLEABILITY_API APlayerCharacter : public ACustomCharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

protected:
    virtual void BeginPlay() override;

public:
    // Composant qui gère toutes les abilities du joueur
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAbilityManagerComponent* AbilityManager;
};
