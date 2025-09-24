// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseProjectile.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CustomCharacter.h"
#include "AbilityEffect.h"
#include "AbilityData.h"

ABaseProjectile::ABaseProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    // Root
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    // Collision
    Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    Collision->SetupAttachment(Root);
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // ou ton custom preset


    Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnProjectileOverlap);

    // Mesh
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
}


void ABaseProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector MoveDir = InitialDirection;

    switch (MovementType)
    {
    case EProjectileMovementType::TowardTarget:
    case EProjectileMovementType::Forward:
        break; // déjà défini dans InitialDirection
    case EProjectileMovementType::Homing:
        if (Target && Target->IsAlive())
        {
            MoveDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
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

    InitialDirection = MoveDir; // garde la dernière direction
}



void ABaseProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (Source == nullptr) return;
    if (OtherActor == Source) return;

    UE_LOG(LogTemp, Warning, TEXT("[Projectile %s] Hit %s (Source=%s) | NbEffects=%d"),
        *GetName(),
        *OtherActor->GetName(),
        Source ? *Source->GetName() : TEXT("null"),
        EffectsOnHit.Num());


    ACustomCharacter* HitCharacter = Cast<ACustomCharacter>(OtherActor);
    if (HitCharacter && HitCharacter != Source)
    {
        FAbilityEffectContext Ctx(Source, HitCharacter, AbilityData, this);

        for (UAbilityEffect* Effect : EffectsOnHit)
        {
            if (Effect && Effect->TriggerPhase == EEffectTriggerPhase::OnHit)
            {
                Effect->ApplyEffect(Ctx);
            }
        }

    }
}