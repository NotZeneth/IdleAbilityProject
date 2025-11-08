// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "PlayerCharacter.h"
#include "WaveGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "ShopWidget.h"

void UGameplayWidget::NativeConstruct()
{
    Super::NativeConstruct();

    PlayerRef = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    //GameModeRef = PlayerRef->GameModeRef;
    GameModeRef = Cast<AWaveGameMode>(UGameplayStatics::GetGameMode(GetWorld())); // + safe au cas ou, sait on jamais


    if (ButtonAddGold)
        ButtonAddGold->OnClicked.AddDynamic(this, &UGameplayWidget::OnAddGold);

    if (ButtonAddGem)
        ButtonAddGem->OnClicked.AddDynamic(this, &UGameplayWidget::OnAddGem);

    if (ButtonJumpWave)
        ButtonJumpWave->OnClicked.AddDynamic(this, &UGameplayWidget::OnJumpWave);

    if (ButtonShop)
        ButtonShop->OnClicked.AddDynamic(this, &UGameplayWidget::OnToggleShop);

}

void UGameplayWidget::UpdateHealth(float CurrentHP, float MaxHP)
{
    if (HealthBar)
        HealthBar->SetPercent(MaxHP > 0 ? CurrentHP / MaxHP : 0.f);

    if (TextHealth)
        TextHealth->SetText(FText::FromString(FString::Printf(TEXT("HP: %.0f / %.0f"), CurrentHP, MaxHP)));
}

void UGameplayWidget::UpdateGold(int Gold)
{
    if (TextGold)
        TextGold->SetText(FText::FromString(FString::Printf(TEXT("Gold: %d"), Gold)));
}

void UGameplayWidget::UpdateGem(int Gem)
{
    if (TextGem)
        TextGem->SetText(FText::FromString(FString::Printf(TEXT("Gem: %d"), Gem)));
}

void UGameplayWidget::UpdateWave(int Wave)
{
    if (TextWave)
        TextWave->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), Wave)));
}

void UGameplayWidget::OnAddGold()
{
    if (PlayerRef)
        PlayerRef->AddGold(1000);
}

void UGameplayWidget::OnAddGem()
{
    if (PlayerRef)
        PlayerRef->AddGem(1000);
}

void UGameplayWidget::OnJumpWave()
{
    FString InputText = InputWave ? InputWave->GetText().ToString() : TEXT("");
    int Wave = FCString::Atoi(*InputText);

    if (Wave <= 0) return;

    GameModeRef->JumpToWave(Wave);
}

void UGameplayWidget::OnToggleShop()
{
    if (!ShopWidgetRef && ShopWidgetClass)
    {
        ShopWidgetRef = Cast<UShopWidget>(CreateWidget(GetWorld(), ShopWidgetClass));
        if (ShopWidgetRef)
        {
            ShopWidgetRef->AddToViewport(10);
        }
    }

    if (ShopWidgetRef)
    {
        bool bShow = (ShopWidgetRef->GetVisibility() != ESlateVisibility::Visible);
        ShopWidgetRef->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

        if (bShow)
        {
            ShopWidgetRef->PopulateShop(); // refresh
        }
    }
}

