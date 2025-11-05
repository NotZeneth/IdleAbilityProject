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

    if (bRotateToVelocity)
    {
        FRotator Rot = InitialDirection.Rotation();
        Rot.Yaw += MeshYawOffsetDeg;
        SetActorRotation(Rot);
    }

    GetWorld()->GetTimerManager().SetTimer(
        LifeTimerHandle,
        this,
        &ABaseProjectile::DestroyProjectile,
        TimeBeforeSelfDestruct,
        false);

    UE_LOG(LogTemp, Verbose, TEXT("[Projectile] BeginPlay terminé pour %s"), *GetName());
}


void ABaseProjectile::InitializeProjectile()
{
    if (MovementType != EProjectileMovementType::Forward && !Target)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Projectile] InitializeProjectile appelé sans Target valide"));
        return;
    }

    switch (MovementType)
    {
        case EProjectileMovementType::TeleportToTarget:
        {
            FVector NewLoc = Target->GetActorLocation() + SpawnOffset;
            SetActorLocation(NewLoc, false, nullptr, ETeleportType::TeleportPhysics);

            UE_LOG(LogTemp, Log, TEXT("[Projectile] Téléporté sur la cible %s"), *Target->GetName());
            break;
        }

        case EProjectileMovementType::TowardTarget:
        {
            InitialDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            break;
        }

        case EProjectileMovementType::Homing:
        {
            const FVector Forward = GetActorForwardVector();
            const FVector Up = FVector::UpVector;
            InitialDirection = (Forward + Up * 0.6f).GetSafeNormal();
            break;
        }

        case EProjectileMovementType::Forward:
        {
            InitialDirection = GetActorForwardVector();
            break;
        }

        default:
            break;
    }

    if (bRotateToVelocity)
    {
        FRotator Rot = InitialDirection.Rotation();
        Rot.Yaw += MeshYawOffsetDeg;
        SetActorRotation(Rot);
    }

}


void ABaseProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Direction mise à jour selon le type de mouvement
    switch (MovementType)
    {
    case EProjectileMovementType::Homing: // 2D quaternion cool mais wah 3d c'est stylish donc je laisse
        if (Target && Target->IsAlive())
        {
            FVector DesiredDir = (Target->GetActorLocation() - GetActorLocation());

            DesiredDir.Y = 0;
            DesiredDir.Normalize();

            FVector CurrentDir = InitialDirection.GetSafeNormal();

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
void ABaseProjectile::OnProjectileOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!Source || OtherActor == Source)
        return;

    ACustomCharacter* HitCharacter = Cast<ACustomCharacter>(OtherActor);
    if (!HitCharacter || !HitCharacter->IsAlive())
        return;

    if (MovementType == EProjectileMovementType::Homing)
    {
        // Si la cible touchée n'est pas celle que le projectile poursuit
        if (HitCharacter != Target)
        {
            if (CanBeBlocked)
            {
                // Le projectile peut être intercepté : il touche l’ennemi et s’arrête ici
                UE_LOG(LogTemp, Log, TEXT("[Projectile] Homing intercepté par %s"), *HitCharacter->GetName());
                Destroy();
            }
            else
            {
                // Projectile ignore les ennemis autres que sa vraie cible
                UE_LOG(LogTemp, Verbose, TEXT("[Projectile] Homing ignore %s (cible = %s)"),
                    *HitCharacter->GetName(),
                    Target ? *Target->GetName() : TEXT("null"));
            }
            return;
        }
    }

    UAbilityManagerComponent* Manager =
        Source ? Source->FindComponentByClass<UAbilityManagerComponent>() : nullptr;

    for (UAbilityEffectData* Effect : EffectsOnHit)
    {
        if (!Effect || Effect->TriggerPhase != EEffectTriggerPhase::OnHit)
            continue;

        FAbilityEffectContext Ctx;
        Ctx.Source = Source;
        Ctx.Target = HitCharacter;
        Ctx.Ability = Ability;
        Ctx.Projectile = this;

        if (Manager)
            Manager->ApplyEffectToTarget(Effect, Ctx);
        else
            Effect->ApplyEffect(Ctx);
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