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
    Collision->InitBoxExtent(FVector(15.f)); // cube de 30x30x30
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    Collision->SetupAttachment(Root);

    Collision->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnProjectileOverlap);

    // Mesh
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseProjectile::BeginPlay()
{
    Super::BeginPlay();

    // Direction initiale
    if (Target && MovementType == EProjectileMovementType::TowardTarget)
    {
        InitialDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    }
    else if (MovementType == EProjectileMovementType::Forward)
    {
        InitialDirection = GetActorForwardVector();
    }
}

void ABaseProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector MoveDir;

    switch (MovementType)
    {
    case EProjectileMovementType::Forward:
    case EProjectileMovementType::TowardTarget:
        MoveDir = InitialDirection;
        break;

    case EProjectileMovementType::Homing:
        if (Target && Target->IsAlive())
        {
            MoveDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        }
        else
        {
            MoveDir = InitialDirection;
        }
        break;
    }

    SetActorLocation(GetActorLocation() + MoveDir * ProjectileSpeed * DeltaTime, true);
}

void ABaseProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (Source == nullptr) return;
    if (OtherActor == Source) return;


    ACustomCharacter* HitCharacter = Cast<ACustomCharacter>(OtherActor);
    if (HitCharacter && HitCharacter != Source)
    {
        // Appliquer tous les effets "OnHit"
        for (UAbilityEffect* Effect : EffectsOnHit)
        {
            if (Effect && Effect->TriggerPhase == EEffectTriggerPhase::OnHit)
            {
                Effect->ApplyEffect(Source, HitCharacter, AbilityData);
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("Projectile hit %s"), *HitCharacter->GetName());

        Destroy();
    }
}