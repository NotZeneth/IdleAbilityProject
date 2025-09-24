// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class ACustomCharacter;
class UAbilityData;
class UAbilityEffect;
class UBoxComponent;

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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* Mesh;


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

    FVector InitialDirection;

    // Orientation helpers
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    bool bRotateToVelocity = true;

    // Si ton mesh ne pointe pas sur +X, mets un offset (ex: 90 si ton mesh pointe +Y)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float MeshYawOffsetDeg = 0.f;


private:

    UFUNCTION()
    void OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};
