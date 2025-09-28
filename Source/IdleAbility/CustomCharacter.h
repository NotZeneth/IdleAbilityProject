// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CustomCharacter.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
    Physical,
    Magical,
    Pure
};

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

    // --- Stats de base ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 TeamId = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHP = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float HpRegenPercent = 0.f; // % par seconde

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Attack = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float PhysicalDmgReduction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MagicalDmgReduction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float IgnoreDmgReduction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CooldownReduction = 0.f; // peut être négatif

    // --- Effets secondaires ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MultishotChance = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 MultishotAmount = 2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BounceChance = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 MaxBounces = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float FrenzyChance = 0.05f;

    // --- Combat ---
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void RegenHealthOverTime(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Heal(float HealAmount);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    bool IsAlive();
};
