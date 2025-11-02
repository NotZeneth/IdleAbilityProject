// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "WaveGameMode.h"
#include "AbilityManagerComponent.h"
#include "AbilityBarWidget.h"
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

    if (AbilityBarClass)
    {
        AbilityBarInstance = CreateWidget<UAbilityBarWidget>(GetWorld(), AbilityBarClass);
        if (AbilityBarInstance)
        {
            AbilityBarInstance->AddToViewport();
        }
    }
}
