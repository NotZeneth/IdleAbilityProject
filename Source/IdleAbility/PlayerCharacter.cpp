// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "AbilityManagerComponent.h"

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
}
