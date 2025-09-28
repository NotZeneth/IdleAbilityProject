// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCharacter.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AbilityManagerComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
    PlayerRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void AEnemyCharacter::TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source)
{
    Super::TakeCustomDamage(DamageAmount, DamageType, Source);

    if (!IsAlive() && PlayerRef && PlayerRef->AbilityManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s est mort -> notify Player"), *GetName());
        PlayerRef->AbilityManager->OnEnemyKilled(this);
        Destroy(); // on détruit ICI
    }
}
