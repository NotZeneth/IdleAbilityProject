// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseProjectile.h"
#include "AreaPulseEffectData.h"
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

    switch (MovementType)
    {
    case EProjectileMovementType::Homing: // j'aurai du 2D / quaternion mais en fait ca rend bien donc je laisse
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
        break;

    case EProjectileMovementType::TowardTarget:
        break;
    }

    FVector NewLoc = GetActorLocation() + InitialDirection * ProjectileSpeed * DeltaTime;

    NewLoc.Y = LockedY;

    SetActorLocation(NewLoc, true);

    if (bRotateToVelocity)
    {
        FRotator Rot = InitialDirection.Rotation();
        Rot.Yaw += MeshYawOffsetDeg;
        SetActorRotation(Rot);
    }
}

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
        if (HitCharacter != Target)
        {
            if (CanBeBlocked)
            {
                UE_LOG(LogTemp, Log, TEXT("[Projectile] Homing intercepté par %s"), *HitCharacter->GetName());
                Destroy();
            }
            else
            {
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

        // Cas spécifique : effet AreaPulse, la je dois le stocker chez le joueur et pas l'enemy
        if (Effect->IsA(UAreaPulseEffectData::StaticClass()))
        {
            Ctx.Target = Source;
        }

        if (Manager)
            Manager->ApplyEffectToTarget(Effect, Ctx);
        else
            Effect->ApplyEffect(Ctx);
    }
}

void ABaseProjectile::DestroyProjectile()
{
    Destroy();
}