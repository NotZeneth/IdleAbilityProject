// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomCharacter.generated.h"

// Bon au final on fait qu'use pure mais whatever
UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Physical,
    Magical,
    Pure
};

// Class commune au joueur et a l'ennemi
UCLASS()
class IDLEABILITY_API ACustomCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACustomCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // joueur = 0, enemy = 1
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Core")
    int32 TeamId = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Core")
    float MaxHP = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Core")
    float CurrentHP = 100.f;

    // La regen c'est un % de la vie
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Core")
    float HpRegenPercent = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Core")
    float Attack = 10.f;

    // On en utilise pas atm mais le code est la
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Defense")
    float PhysicalDmgReduction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Defense")
    float MagicalDmgReduction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Defense")
    float IgnoreDmgReduction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 MaxBounces = 3;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void RegenHealthOverTime(float DeltaTime);
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void Heal(float HealAmount);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool IsAlive();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
    bool isFrozen = false;

    // bonus de dégâts reçus (0.5 = +50% dégâts)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
    float DamageTakenBonus = 0.f;


    UPROPERTY()
    class AWaveGameMode* GameModeRef = nullptr;
};
