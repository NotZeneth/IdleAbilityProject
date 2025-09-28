// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomCharacter.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class IDLEABILITY_API AEnemyCharacter : public ACustomCharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source) override;

private:
    UPROPERTY()
    class APlayerCharacter* PlayerRef = nullptr;
};
