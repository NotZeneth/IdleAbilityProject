// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseProjectile.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CustomCharacter.h"
#include "AbilityEffectData.h"
#include "AbilityData.h"
#include "AbilityManagerComponent.h"

ABaseProjectile::ABaseProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    Collision->SetupAttachment(Root);
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnProjectileOverlap);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (Target && MovementType == EProjectileMovementType::TowardTarget)
    {
        InitialDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    }
    else if (MovementType == EProjectileMovementType::Forward)
    {
        InitialDirection = GetActorForwardVector();
    }

    if (bRotateToVelocity)
    {
        FRotator Rot = InitialDirection.Rotation();
        Rot.Yaw += MeshYawOffsetDeg;
        SetActorRotation(Rot);
    }

    // Démarre le timer qui appelle DestroyProjectile après LifeTime secondes
    GetWorld()->GetTimerManager().SetTimer(
        LifeTimerHandle,
        this,
        &ABaseProjectile::DestroyProjectile,
        TimeBeforeSelfDestruct,
        false
    );
}

void ABaseProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector MoveDir = InitialDirection;

    switch (MovementType)
    {
    case EProjectileMovementType::TowardTarget:
    case EProjectileMovementType::Forward:
        break; // Already set in BeginPlay
    case EProjectileMovementType::Homing:
        if (Target && Target->IsAlive())
        {
            MoveDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        }
        else
        {
            MovementType = EProjectileMovementType::Forward; // Fallback
        }
        break;
    }

    SetActorLocation(GetActorLocation() + MoveDir * ProjectileSpeed * DeltaTime, true);

    if (bRotateToVelocity)
    {
        FRotator Rot = MoveDir.Rotation();
        Rot.Yaw += MeshYawOffsetDeg;
        SetActorRotation(Rot);
    }

    InitialDirection = MoveDir; // Keep last valid direction
}

// Base projectile s'occupe de trigger le on hit effect
void ABaseProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!Source || OtherActor == Source) return;

    ACustomCharacter* HitCharacter = Cast<ACustomCharacter>(OtherActor);
    if (HitCharacter && HitCharacter != Source && HitCharacter->IsAlive())
    {
        UAbilityManagerComponent* Manager =
            Source ? Source->FindComponentByClass<UAbilityManagerComponent>() : nullptr;

        for (UAbilityEffectData* Effect : EffectsOnHit) // ou SubEffects si tu utilises ça
        {
            if (!Effect || Effect->TriggerPhase != EEffectTriggerPhase::OnHit)
                continue;

            FAbilityEffectContext Ctx;
            Ctx.Source = Source;
            Ctx.Target = HitCharacter;
            Ctx.Ability = Ability;
            Ctx.Projectile = this;

            if (Manager)
                Manager->ApplyEffectToTarget(Effect, Ctx);  // gère instantané + persistant
            else
                Effect->ApplyEffect(Ctx); // fallback (pas de persistance si pas de manager)
        }
    }
}

void ABaseProjectile::RedirectToTarget(ACustomCharacter* NewTarget) 
{
    if (!NewTarget) return;

    Target = NewTarget;

    // recalcul d’une direction valide et on force un comportement "homing"
    MovementType = EProjectileMovementType::Homing;
    InitialDirection = (NewTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();

    if (bRotateToVelocity)
    {
        FRotator Rot = InitialDirection.Rotation();
        Rot.Yaw += MeshYawOffsetDeg;
        SetActorRotation(Rot);
    }
}

void ABaseProjectile::DestroyProjectile()
{
    Destroy();
}