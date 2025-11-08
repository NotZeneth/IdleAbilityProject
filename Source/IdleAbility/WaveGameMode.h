// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WaveGameMode.generated.h"

class AEnemyCharacter;
class APlayerCharacter;

UCLASS()
class IDLEABILITY_API AWaveGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:

     // point de spawn des mobs
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves|Spawning")
    FVector SpawnLocation = FVector::ZeroVector;

    // base
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves|Spawning")
    TSubclassOf<AEnemyCharacter> EnemyClass;

    // miniboss 5, 15, 25...
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves|Spawning")
    TSubclassOf<AEnemyCharacter> MiniBossClass;

    // boss 10, 20, 30....
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves|Spawning")
    TSubclassOf<AEnemyCharacter> BossClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves|Spawning")
    int32 EnemiesPerWave = 4;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves|Spawning")
    float SpawnInterval = 0.75f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves|Timing")
    float TimeBetweenWaves = 3.0f;


    UPROPERTY(BlueprintReadOnly, Category = "Waves|State")
    int32 CurrentWave = 0;

    int32 EnemiesLeftToSpawn = 0;

    int32 EffectiveNumberToSpawnThisWave = 4;

    UPROPERTY(BlueprintReadOnly, Category = "Waves|State")
    TArray<AEnemyCharacter*> EnemyList;

    UPROPERTY(BlueprintReadOnly, Category = "References")
    APlayerCharacter* PlayerRef = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
    FVector WallLocation = FVector::ZeroVector;




    UFUNCTION(BlueprintCallable, Category = "Waves|Control")
    void StartSpawning();

    UFUNCTION(BlueprintCallable, Category = "Waves|Control")
    void JumpToWave(int NewWave);

    UFUNCTION(BlueprintCallable, Category = "Waves|Control")
    void HandlePlayerDeath();

    UFUNCTION()
    void OnEnemyDied(AEnemyCharacter* DeadEnemy);

protected:

    void StartWave();

    UFUNCTION()
    void SpawnEnemy();

    UPROPERTY()
    FTimerHandle SpawnTimerHandle;
};
