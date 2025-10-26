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

protected:
    virtual void BeginPlay() override;

public:
    virtual void TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source) override;

private:
    UPROPERTY()
    class APlayerCharacter* PlayerRef = nullptr;
};
