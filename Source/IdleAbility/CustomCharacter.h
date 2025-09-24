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
    // Sets default values for this character's properties
    ACustomCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // --- Faction / équipe ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    int32 TeamId = 0; 

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHP;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float HpRegenPercent; // %  regen par sec

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Attack;

public:


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float PhysicalDmgReduction; // 0.2 = 20% reduc

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MagicalDmgReduction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float IgnoreDmgReduction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CooldownReduction; // Can be negative, increasing cooldown

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void RegenHealthOverTime(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Heal(float HealAmount);

    UFUNCTION(BlueprintCallable, Category = "Stats")
    bool IsAlive();

public: 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MultishotChance = 0.1f; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 MultishotAmount = 2; 


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BounceChance = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float FrenzyChance = 0.05f;
};
