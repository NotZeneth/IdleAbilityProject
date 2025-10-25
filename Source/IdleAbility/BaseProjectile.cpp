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

    LockedY = GetActorLocation().Y;

    if (Target && MovementType == EProjectileMovementType::Homing)
    {
        // Tir en cloche au départ
        const FVector Forward = GetActorForwardVector();
        const FVector Up = FVector::UpVector;
        InitialDirection = (Forward + Up * 0.6f).GetSafeNormal();
    }
    else if (Target && MovementType == EProjectileMovementType::TowardTarget)
    {
        // Direction initiale vers la cible
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

    GetWorld()->GetTimerManager().SetTimer(
        LifeTimerHandle, this, &ABaseProjectile::DestroyProjectile, TimeBeforeSelfDestruct, false);
}


void ABaseProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Direction mise à jour selon le type de mouvement
    switch (MovementType)
    {
    case EProjectileMovementType::Homing:
        if (Target && Target->IsAlive())
        {
            FVector DesiredDir = (Target->GetActorLocation() - GetActorLocation());

            // Ignorer la profondeur pour gameplay 2D side-view
            DesiredDir.Y = 0;
            DesiredDir.Normalize();

            FVector CurrentDir = InitialDirection.GetSafeNormal();

            // rotation lissée (vitesse angulaire max)
            FRotator CurRot = CurrentDir.Rotation();
            FRotator DesRot = DesiredDir.Rotation();
            FRotator NewRot = FMath::RInterpConstantTo(CurRot, DesRot, DeltaTime, MaxTurnRateDeg);

            InitialDirection = NewRot.Vector().GetSafeNormal();
        }
        else
        {
            MovementType = EProjectileMovementType::Forward;
        }
        break;

    case EProjectileMovementType::Forward:
        // rien à changer, on garde la direction initiale
        break;

    case EProjectileMovementType::TowardTarget:
        // rien non plus ici, tu veux du straight-to-target sans adaptation
        break;
    }

    // Mouvement
    FVector NewLoc = GetActorLocation() + InitialDirection * ProjectileSpeed * DeltaTime;

    //  Lock profondeur Y -> vue de profil 2D
    NewLoc.Y = LockedY;

    SetActorLocation(NewLoc, true);

    // Rotation pour aligner le mesh sur le mouvement
    if (bRotateToVelocity)
    {
        FRotator Rot = InitialDirection.Rotation();
        Rot.Yaw += MeshYawOffsetDeg;
        SetActorRotation(Rot);
    }
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