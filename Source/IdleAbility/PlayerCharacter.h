// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomCharacter.h"
#include "PlayerCharacter.generated.h"

class UAbilityManagerComponent;
class UAbilityBarWidget;
class UGameplayWidget; // oui je sais les def unique je peux ecrire class dans la déclaration, mais ca m'aide a pas oublier d'inclure dans le cpp

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UAbilityBarWidget> AbilityBarClass;

    UPROPERTY()
    UAbilityBarWidget* AbilityWidgetRef;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UGameplayWidget> GameplayWidgetClass;

    UPROPERTY()
    UGameplayWidget* GameplayWidgetRef;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int CurrentGold = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int GoldMultiplier = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int CurrentGem = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int GemMultiplier = 1;

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void AddGold(float amount);

    UFUNCTION(BlueprintCallable, Category = "Progression")
    void AddGem(float amount);

    virtual void TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source) override;
};
