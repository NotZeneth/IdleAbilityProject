// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveGameMode.h"
#include "EnemyCharacter.h"
#include "PlayerCharacter.h"
#include "AbilityManagerComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameplayWidget.h"
#include "Kismet/KismetMathLibrary.h"

void AWaveGameMode::StartSpawning()
{
    FTimerHandle Handle;
    GetWorldTimerManager().SetTimer(Handle, this, &AWaveGameMode::StartWave, 1, false);
}

void AWaveGameMode::BeginPlay()
{
    Super::BeginPlay();
    StartSpawning();
}

void AWaveGameMode::StartWave()
{
    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Wave] EnemyClass is null"));
        return;
    }

    ++CurrentWave;
    PlayerRef->GameplayWidgetRef->UpdateWave(CurrentWave);
    EnemiesLeftToSpawn = EnemiesPerWave;

    UE_LOG(LogTemp, Log, TEXT("[Wave] Start wave %d: spawning %d enemies"), CurrentWave, EnemiesLeftToSpawn);
    SpawnEnemy();

    // Ca ca marchait aussi, c'est + simple mais on peut pas garder la ref du handle
    
    //for (int32 i = 0; i < EnemiesPerWave; ++i)
    //{
    //    FTimerHandle TimerHandle; 
    //    GetWorldTimerManager().SetTimer(TimerHandle, this, &AWaveGameMode::SpawnEnemy, i * SpawnInterval, false);
    //}
}

void AWaveGameMode::SpawnEnemy()
{
    TSubclassOf<AEnemyCharacter> ClassToSpawn = EnemyClass;

    const int32 LastDigit = CurrentWave % 10;

    if (LastDigit == 5 && MiniBossClass)
    {
        ClassToSpawn = MiniBossClass;
        EffectiveNumberToSpawnThisWave = 4; 
    }
    else if (LastDigit == 0 && BossClass)
    {
        ClassToSpawn = BossClass;
        EffectiveNumberToSpawnThisWave = 1; // forcer un seul spawn
    }
    else
    {
        EffectiveNumberToSpawnThisWave = EnemiesPerWave;
    }

    if (!ClassToSpawn) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AEnemyCharacter* Enemy = GetWorld()->SpawnActor<AEnemyCharacter>(ClassToSpawn, SpawnLocation, FRotator(0, 180, 0), Params);
    if (!Enemy)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Wave] Failed to spawn enemy"));
        return;
    }

    Enemy->GameModeRef = this;
    EnemyList.Add(Enemy);
    Enemy->ConfigStats(CurrentWave);

    UE_LOG(LogTemp, Log, TEXT("[Wave] Spawned %s (Wave %d)"), *ClassToSpawn->GetName(), CurrentWave);

    EnemiesLeftToSpawn -= 1;
    if (EnemiesLeftToSpawn > 0)
    {
        GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AWaveGameMode::SpawnEnemy, SpawnInterval, false);
    }
}


void AWaveGameMode::OnEnemyDied(AEnemyCharacter* DeadEnemy) // called by the enemy itself
{
    EnemyList.Remove(DeadEnemy); // j'pense pending destroy donc ca permet quand meme de retirer sans probleme

    // L'enemie pourrait call au joueur, mais c'est pas vraiment sa responsabilité je pense
    PlayerRef->AddGold(DeadEnemy->GoldOnDeath);
    PlayerRef->AddGem(DeadEnemy->GemOnDeath);
    PlayerRef->AbilityManager->OnEnemyKilled(DeadEnemy);

    if (EnemiesLeftToSpawn == 0 && EnemyList.IsEmpty()) // Double check car un enemie peut se faire OS avant le spawn du prochain
    {
        FTimerHandle Handle;
        GetWorldTimerManager().SetTimer(Handle, this, &AWaveGameMode::StartWave, TimeBetweenWaves, false);
        UE_LOG(LogTemp, Log, TEXT("[Wave] Wave %d completed. Next wave in %.2fs"), CurrentWave, TimeBetweenWaves);

        PlayerRef->AbilityManager->ResetAllEffectsAndCooldowns();
    }
}
void AWaveGameMode::JumpToWave(int NewWave)
{
    if (!PlayerRef || !PlayerRef->AbilityManager)
        return;
    if (NewWave <= 0)
        return;

    UE_LOG(LogTemp, Warning, TEXT("[Wave] Jump vers la vague %d (reset des ennemis et effets)"), NewWave);

    // Copie locale pour éviter la modification concurrente
    TArray<AEnemyCharacter*> EnemiesSnapshot = EnemyList;

    CurrentWave = NewWave - 1;

    for (AEnemyCharacter* Enemy : EnemiesSnapshot)
    {
        if (!Enemy || !Enemy->IsAlive())
                    continue;

        // Oui non j'aurai juste du créer une fonction pour tuer sans declancher le loot, ou juste un param mais manque de temps
        Enemy->GoldOnDeath = 0;
        Enemy->GemOnDeath = 0;
        Enemy->TakeCustomDamage(999999.f, EDamageType::Pure, PlayerRef);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Wave] JumpToWave terminé (ennemis nettoyés)"));

    EnemyList.Empty();
}

void AWaveGameMode::HandlePlayerDeath()
{
    // Juste au cas où
    if (CurrentWave <= 1)
    {
        JumpToWave(1);
        return;
    }

    int32 TargetWave = CurrentWave;

    // si on est sur une vague multiple de 5 (5, 10, 15, 20...) -> revenir à la précédente
    if (CurrentWave % 5 == 0)
    {
        TargetWave = FMath::Max(1, CurrentWave - 1);
    }

    UE_LOG(LogTemp, Warning, TEXT("[Wave] Player died on wave %d -> restarting wave %d"), CurrentWave, TargetWave);
    PlayerRef->CurrentHP = PlayerRef->MaxHP;
    PlayerRef->Heal(1);
    JumpToWave(TargetWave);
}

