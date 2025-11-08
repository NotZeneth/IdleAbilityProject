// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class ACustomCharacter;
class UAbilityData;
class UAbilityEffectData;
class UBoxComponent;

// Type de déplacement du projectile
UENUM(BlueprintType)
enum class EProjectileMovementType : uint8
{
    Forward,          // avance tout droit
    TowardTarget,     // se dirige en ligne droite vers la cible
    Homing,           // homing avec genre une trajectoire courbée
    TeleportToTarget  // teleporte sur la cible instant
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

    // setup initial
    UFUNCTION()
    void InitializeProjectile();

    void DestroyProjectile();

    // redirection sur une nouvelle cible (bounce)
    void RedirectToTarget(ACustomCharacter* NewTarget);

    UFUNCTION()
    void OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Components")
    USceneComponent* Root = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Components")
    UBoxComponent* Collision = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Components")
    UStaticMeshComponent* Mesh = nullptr;

    UPROPERTY()
    ACustomCharacter* Source = nullptr;

    UPROPERTY()
    ACustomCharacter* Target = nullptr;

    UPROPERTY()
    const UAbilityData* Ability = nullptr;

    UPROPERTY()
    TArray<UAbilityEffectData*> EffectsOnHit;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Movement")
    float ProjectileSpeed = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Movement")
    EProjectileMovementType MovementType = EProjectileMovementType::Forward;

    FVector InitialDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Movement")
    bool bRotateToVelocity = true;

    // offset de rotation du mesh (pour compenser orientation modèle)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Movement")
    float MeshYawOffsetDeg = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Lifetime")
    float TimeBeforeSelfDestruct = 20.f;

    // offset de spawn pour le multishot pour faire beau, mais au final pas vrmt le time
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Spawn")
    FVector SpawnOffset = FVector(0.f, 0.f, 50.f);

    float LockedY = 0.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Bounce")
    int32 RemainingBounces = 0;

    // angle max per sec for homing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Homing")
    float MaxTurnRateDeg = 360.f;

    // Si un projectile peut traverser l'enemie ou s'arrête au premier touché
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Collision")
    bool CanBeBlocked = false;

private:

    FTimerHandle LifeTimerHandle;
};
