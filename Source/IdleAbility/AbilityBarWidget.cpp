// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityBarWidget.h"
#include "AbilityButtonWidget.h"
#include "Components/HorizontalBox.h"
#include "AbilityManagerComponent.h"
#include "PlayerCharacter.h"

void UAbilityBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Au cas ou pas encore set pour le player, même si logiquement si, on le recup comme ca
    if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        AbilityManager = Player->FindComponentByClass<UAbilityManagerComponent>();
    }

    RefreshButtons(); // on cree les boutons, ca sera called par ceux qui veulent modif plus tard genre le shop
}

void UAbilityBarWidget::RefreshButtons()
{
    if (!AbilityManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AbilityBar] var AbilityManager invalide aled"));
        return;
    }

    if (!ButtonContainer)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AbilityBar] Var ButtonContainer invalide"));
        return;
    }

    if (!AbilityButtonClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[AbilityBar] var AbilityButtonClass invalide"));
        return;
    }

    // on clear avant de regenerer
    ButtonContainer->ClearChildren();

    const int32 NumAbilities = AbilityManager->EquippedAbilities.Num();
    UE_LOG(LogTemp, Log, TEXT("[AbilityBar] Création de %d boutons"), NumAbilities);

    for (int32 i = 0; i < NumAbilities; ++i)
    {
        const FAbilitySpec& Spec = AbilityManager->EquippedAbilities[i];
        if (!Spec.Ability || !Spec.bUnlocked)
            continue;

        if (UAbilityButtonWidget* Button = CreateWidget<UAbilityButtonWidget>(GetWorld(), AbilityButtonClass))
        {
            Button->AbilityIndex = i;
            Button->AbilityManager = AbilityManager;
            ButtonContainer->AddChildToHorizontalBox(Button);
        }
    }
}
