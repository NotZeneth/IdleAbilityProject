// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomCharacter.h"
#include "PlayerCharacter.generated.h"

class UAbilityManagerComponent;
class UAbilityBarWidget;
class UGameplayWidget;

UCLASS()
class IDLEABILITY_API APlayerCharacter : public ACustomCharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

protected:
    virtual void BeginPlay() override;

public:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAbilityManagerComponent* AbilityManager = nullptr;

     // --------------- UI

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UAbilityBarWidget> AbilityBarClass;

    UPROPERTY()
    UAbilityBarWidget* AbilityWidgetRef = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UGameplayWidget> GameplayWidgetClass;

    UPROPERTY()
    UGameplayWidget* GameplayWidgetRef = nullptr;

    // --------------- Stats

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats") 
    float AttackMultiplier = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float HealthMultiplier = 1.f;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    float GetPlayerAttack(); // bon en vrai y a que le damage effect qui peut mettre des degats donc overkill mais ez

    virtual void TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source) override;

    virtual void Heal(float HealAmount) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int CurrentGold = 0;

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
};
