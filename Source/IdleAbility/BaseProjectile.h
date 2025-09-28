// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class ACustomCharacter;
class UAbilityData;
class UAbilityEffectData;
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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UBoxComponent* Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* Mesh;

    UPROPERTY()
    ACustomCharacter* Source = nullptr;

    UPROPERTY()
    ACustomCharacter* Target = nullptr;

    UPROPERTY()
    const UAbilityData* Ability = nullptr;

    UPROPERTY()
    TArray<UAbilityEffectData*> EffectsOnHit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile");
    float ProjectileSpeed = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    EProjectileMovementType MovementType = EProjectileMovementType::Forward;

    FVector InitialDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    bool bRotateToVelocity = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float MeshYawOffsetDeg = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float TimeBeforeSelfDestruct = 20.f;

    void RedirectToTarget(ACustomCharacter* NewTarget);

    FTimerHandle LifeTimerHandle;
    void DestroyProjectile();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bounce")
    int32 RemainingBounces = 0;

private:
    UFUNCTION()
    void OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};
