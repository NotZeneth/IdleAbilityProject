// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class IDLEABILITY_API AEnemyCharacter : public ACustomCharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:

    // Racine pour les meshes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    USceneComponent* VisualRoot = nullptr;

    // Mesh genre me skin
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UStaticMeshComponent* EnemyMeshPlane = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual|Status")
    UStaticMeshComponent* FrozenPlane = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual|Status") // on l'utilise pas au final j'crois, trop de dots differentes et flemme
    UStaticMeshComponent* BurningPlane = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual|Status")
    UStaticMeshComponent* WeakenedPlane = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual|UI")
    class UTextRenderComponent* HealthTextComponent = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Visual|UI")
    void UpdateHealthText();

    // ------------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Core")
    float MoveSpeed = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Core")
    float DamagePerSec = 10.f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Core")
    float AttackRange = 100.f;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Rewards")
    float GoldOnDeath = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Rewards")
    float GemOnDeath = 0.01f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Rewards")
    float FullGemOnDeathChance = 0.01f;

    // ------------------------------------------------------------

    UFUNCTION(BlueprintCallable, Category = "Enemy")
    void ConfigStats(int Wave);

    virtual void Heal(float HealAmount) override;

    virtual void TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source) override;

private:
    UPROPERTY()
    class APlayerCharacter* PlayerRef = nullptr;
};
