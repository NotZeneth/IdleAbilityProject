// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityBarWidget.h"
#include "AbilityButtonWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "CustomCharacter.h"
#include "AbilityManagerComponent.h"
#include "PlayerCharacter.h"

void UAbilityBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Récupère le AbilityManager du joueur
    if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        AbilityManager = Player->FindComponentByClass<UAbilityManagerComponent>();
    }

    // Création initiale des boutons
    RefreshButtons();
}

void UAbilityBarWidget::RefreshButtons()
{
    if (!AbilityManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AbilityBar] Aucun AbilityManager trouve"));
        return;
    }

    if (!ButtonContainer)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AbilityBar] Aucun container trouve (ButtonContainer)"));
        return;
    }

    if (!AbilityButtonClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AbilityBar] AbilityButtonClass non defini"));
        return;
    }

    // Nettoie d'abord le container
    ButtonContainer->ClearChildren();

    const int32 NumAbilities = AbilityManager->EquippedAbilities.Num();
    UE_LOG(LogTemp, Log, TEXT("[AbilityBar] Création de %d boutons d'abilities"), NumAbilities);

    for (int32 i = 0; i < NumAbilities; ++i)
    {
        UAbilityButtonWidget* Button = CreateWidget<UAbilityButtonWidget>(GetWorld(), AbilityButtonClass);
        if (!Button) continue;

        // Assigne l’index et le manager (important)
        Button->AbilityIndex = i;
        Button->AbilityManager = AbilityManager;

        // Ajoute dans le container horizontal
        ButtonContainer->AddChildToHorizontalBox(Button);
    }
}
