// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCharacter.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "WaveGameMode.h"
#include "AbilityManagerComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
    // === VISUAL ROOT ===
    VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
    VisualRoot->SetupAttachment(RootComponent);

    // === MAIN ENEMY MESH PLANE ===
    EnemyMeshPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMeshPlane"));
    EnemyMeshPlane->SetupAttachment(VisualRoot);
    EnemyMeshPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EnemyMeshPlane->SetHiddenInGame(false);

    // === STATUS EFFECT PLANES ===
    FrozenPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrozenPlane"));
    FrozenPlane->SetupAttachment(VisualRoot);
    FrozenPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    FrozenPlane->SetHiddenInGame(true);

    BurningPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BurningPlane"));
    BurningPlane->SetupAttachment(VisualRoot);
    BurningPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BurningPlane->SetHiddenInGame(true);

    WeakenedPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeakenedPlane"));
    WeakenedPlane->SetupAttachment(VisualRoot);
    WeakenedPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeakenedPlane->SetHiddenInGame(true);
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (isFrozen)
        return;

    FVector Forward = GetActorForwardVector();
    FVector NewLocation = GetActorLocation() + Forward * MoveSpeed * DeltaTime;

    SetActorLocation(NewLocation, true); // true car j'veux que ca stop vs le mur

    // On attaque chaque frame oui, pas d'attack speed + cd etc, flemme, les mobs ont une aura qui dps, the end.
    if (NewLocation.X < GameModeRef->WallLocation.X + AttackRange && GameModeRef->WallHealth>0) // si entre le mur et "mur + max attack range"
    {
        GameModeRef->WallHealth -= DamagePerSec * DeltaTime;
    }
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    const float RandomYOffset = FMath::FRandRange(0.001f, 0.01f);
    FVector NewLoc = VisualRoot->GetRelativeLocation();
    NewLoc.Y += RandomYOffset;
    VisualRoot->SetRelativeLocation(NewLoc);
}

void AEnemyCharacter::TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source)
{
    Super::TakeCustomDamage(DamageAmount, DamageType, Source);

    if (!IsAlive() && PlayerRef && PlayerRef->AbilityManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s est mort -> notify Player"), *GetName());
        PlayerRef->AbilityManager->OnEnemyKilled(this); // Bon en vrai, ca ca pourrait être fait par le manager mais bon heu le temps
        GameModeRef->OnEnemyDied(this);
        Destroy(); // on détruit ICI
    }
}
