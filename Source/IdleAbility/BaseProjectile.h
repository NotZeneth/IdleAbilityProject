// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class ACustomCharacter;
class UAbilityData;
class UAbilityEffect;

UENUM(BlueprintType)
enum class EProjectileMovementType : uint8
{
    Forward,
    TowardTarget,
    Homing
};

UCLASS()
class IDLEABILITY_API ABaseProjectile : public AActor
{
    GENERATED_BODY()

public:
    ABaseProjectile();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    USceneComponent* Root;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component");
    class UBoxComponent* Collision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component");
    class UStaticMeshComponent* Mesh;

    // ---- Data ----
    UPROPERTY()
    ACustomCharacter* Source;

    UPROPERTY()
    ACustomCharacter* Target;

    UPROPERTY()
    const UAbilityData* AbilityData;

    UPROPERTY()
    TArray<UAbilityEffect*> EffectsOnHit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float ProjectileSpeed = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    EProjectileMovementType MovementType = EProjectileMovementType::Forward;

private:
    FVector InitialDirection;

    UFUNCTION()
    void OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};
