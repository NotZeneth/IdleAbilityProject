// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCharacter.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "WaveGameMode.h"
#include "AbilityManagerComponent.h"
#include "Components/TextRenderComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
    VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
    VisualRoot->SetupAttachment(RootComponent);

    EnemyMeshPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMeshPlane"));
    EnemyMeshPlane->SetupAttachment(VisualRoot);
    EnemyMeshPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EnemyMeshPlane->SetHiddenInGame(false);

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

    HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthText"));
    HealthTextComponent->SetupAttachment(RootComponent);
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

void AEnemyCharacter::ConfigStats(int Wave)
{
    const float BaseHP = MaxHP;
    const float BaseDamage = DamagePerSec;
    const float BaseGold = GoldOnDeath;
    const float BaseGem = GemOnDeath;

    // Maths
    const int WavesPerCycle = 10;
    const float GlobalStepMultiplier = 1.3f;   
    const float InCycleGrowth = 0.08f;

    const int CycleIndex = (Wave - 1) / WavesPerCycle;   // 0 pour vagues 1-10, 1 pour 11-20...
    const int WaveInCycle = (Wave - 1) % WavesPerCycle;

    // croissance globale par palier
    const float GlobalMultiplier = FMath::Pow(GlobalStepMultiplier, CycleIndex);

    // croissance douce à l'intérieur du palier (commence lentement, accélère vers la fin)
    const float LocalMultiplier = 1.0f + InCycleGrowth * FMath::Pow(WaveInCycle / (float)WavesPerCycle, 1.5f) * 10.f;

    // multiplicateur final
    const float HPScale = GlobalMultiplier * LocalMultiplier;
    const float DmgScale = GlobalMultiplier * (0.7f + 0.3f * LocalMultiplier);
    const float GoldScale = GlobalMultiplier * (0.8f + 0.5f * LocalMultiplier);
    const float GemScale = GlobalMultiplier * (0.5f + 0.3f * LocalMultiplier);

    MaxHP = FMath::CeilToFloat(BaseHP * HPScale / 10.f) * 10.f;
    DamagePerSec = FMath::CeilToFloat(BaseDamage * DmgScale);
    CurrentHP = MaxHP;
    GoldOnDeath = FMath::FloorToFloat(BaseGold * GoldScale);
    GemOnDeath = FMath::FloorToFloat(BaseGem * GemScale);

    GemOnDeath += (FMath::FRand() <= FullGemOnDeathChance) ? 1.f : 0.f;


    UpdateHealthText();
    UE_LOG(LogTemp, Warning, TEXT("[Wave %d] Cycle %d | HPx%.2f Dmgx%.2f Goldx%.2f"),
        Wave, CycleIndex, HPScale, DmgScale, GoldScale);
}

void AEnemyCharacter::UpdateHealthText()
{
    const FString HPText = FString::Printf(TEXT("%.0f / %.0f"), CurrentHP, MaxHP);
    HealthTextComponent->SetText(FText::FromString(HPText));

}

void AEnemyCharacter::Heal(float HealAmount)
{
    if (!IsAlive()) return;

    CurrentHP = FMath::Clamp(CurrentHP + HealAmount, 0.f, MaxHP);
    UpdateHealthText();
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
    if (NewLocation.X < GameModeRef->WallLocation.X + AttackRange && GameModeRef->PlayerRef->CurrentHP>0) // si entre le mur et "mur + max attack range"
    {
        GameModeRef->PlayerRef->TakeCustomDamage(DamagePerSec * DeltaTime, EDamageType::Pure, this);
    }
}

void AEnemyCharacter::TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source)
{
    Super::TakeCustomDamage(DamageAmount, DamageType, Source);

    UpdateHealthText();

    if (!IsAlive() && PlayerRef && PlayerRef->AbilityManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s est mort -> notify Player"), *GetName());
        GameModeRef->OnEnemyDied(this);
        Destroy(); // on détruit ICI
    }
}
