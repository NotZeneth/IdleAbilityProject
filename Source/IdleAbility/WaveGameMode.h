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

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves")
    FVector SpawnLocation;

    // Classe des ennemis standards
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves")
    TSubclassOf<AEnemyCharacter> EnemyClass;

    // Classe des mini-boss (vagues se terminant par 5)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves")
    TSubclassOf<AEnemyCharacter> MiniBossClass;

    // Classe des boss (vagues se terminant par 0)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves")
    TSubclassOf<AEnemyCharacter> BossClass;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves")
    int32 EnemiesPerWave = 4;

    int32 EffectiveNumberToSpawnThisWave = 4;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves")
    float SpawnInterval = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Waves")
    float TimeBetweenWaves = 3.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Waves")
    int32 CurrentWave = 0;

    UFUNCTION(BlueprintCallable, Category = "Waves")
    void StartSpawning();

    UFUNCTION() // Je pourrai mettre aactor mais + simple comme ca la
    void OnEnemyDied(AEnemyCharacter* DeadEnemy);

    UPROPERTY(BlueprintReadOnly, Category = "Helper")
    TArray<AEnemyCharacter*> EnemyList;

    UPROPERTY(BlueprintReadOnly, Category = "Helper")
    APlayerCharacter* PlayerRef;

// Gameplay
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    FVector WallLocation = FVector(0,0,0);

    UFUNCTION(BlueprintCallable, Category = "Waves")
    void JumpToWave(int NewWave);



protected :

    virtual void BeginPlay() override;

    void StartWave();

    UFUNCTION() 
    void SpawnEnemy();

    UPROPERTY()
    FTimerHandle SpawnTimerHandle;

    int32 EnemiesLeftToSpawn = 0;

};
