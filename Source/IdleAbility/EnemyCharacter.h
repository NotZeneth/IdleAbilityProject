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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    USceneComponent* VisualRoot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UStaticMeshComponent* EnemyMeshPlane;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UStaticMeshComponent* FrozenPlane;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UStaticMeshComponent* BurningPlane;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UStaticMeshComponent* WeakenedPlane;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MoveSpeed = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float DamagePerSec = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackRange = 100.0f;

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaTime) override;

public:
    virtual void TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source) override;

private:
    UPROPERTY()
    class APlayerCharacter* PlayerRef = nullptr;
};
