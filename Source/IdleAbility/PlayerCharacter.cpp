// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "WaveGameMode.h"
#include "AbilityManagerComponent.h"
#include "AbilityBarWidget.h"
#include "GameplayWidget.h"
#include "Blueprint/UserWidget.h"


APlayerCharacter::APlayerCharacter()
{
    // On crée et attache le composant AbilityManager
    AbilityManager = CreateDefaultSubobject<UAbilityManagerComponent>(TEXT("AbilityManager"));
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (!AbilityManager)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerCharacter %s n’a pas d’AbilityManager !"), *GetName());
    }
    AbilityManager->PlayerRef = this;

    if (UWorld* World = GetWorld())
    {
        GameModeRef = Cast<AWaveGameMode>(World->GetAuthGameMode());
        if (!GameModeRef)
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerCharacter %s n’a pas de game mode ref, ca va coincer !"), *GetName());
            return;
        }
        GameModeRef->PlayerRef = this;
    }

    if (AbilityBarClass) // widget barre d'action
    {
        AbilityWidgetRef = CreateWidget<UAbilityBarWidget>(GetWorld(), AbilityBarClass);
        if (AbilityWidgetRef)
        {
            AbilityWidgetRef->AddToViewport();
        }
    }

    if (GameplayWidgetClass)
    {
        GameplayWidgetRef = CreateWidget<UGameplayWidget>(GetWorld(), GameplayWidgetClass);
        if (GameplayWidgetRef)
        {
            GameplayWidgetRef->AddToViewport(1); // au dessus la bar, si jamais

            UE_LOG(LogTemp, Warning, TEXT("Menu UI ajouté au viewport pour %s"), *GetName());

            GameplayWidgetRef->UpdateHealth(CurrentHP, MaxHP);

            GameplayWidgetRef->UpdateGold(CurrentGold);
            GameplayWidgetRef->UpdateGem(CurrentGem);
        }
    }


    // Oui je sais, il faut faire ca dans le player controller, mais j'ai la flemme d'en creer un juste pour ca :X
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;
    }
}

void APlayerCharacter::TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source)
{
    Super::TakeCustomDamage(DamageAmount, DamageType, Source);

    GameplayWidgetRef->UpdateHealth(CurrentHP, MaxHP);
}

void APlayerCharacter::AddGold(float amount)
{
    CurrentGold += amount * GoldMultiplier;
    GameplayWidgetRef->UpdateGold(CurrentGold);
}

void APlayerCharacter::AddGem(float amount)
{
    CurrentGem += amount * GemMultiplier;
    GameplayWidgetRef->UpdateGem(CurrentGem);
}

float APlayerCharacter::GetPlayerAttack()
{
    return Attack * AttackMultiplier;
}