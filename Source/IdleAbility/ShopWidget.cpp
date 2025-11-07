// Fill out your copyright notice in the Description page of Project Settings.

#include "ShopWidget.h"

#include "AbilityData.h"
#include "AbilityManagerComponent.h"
#include "PlayerCharacter.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

// ---------- UShopUpgradeButton ----------
void UShopUpgradeButton::HandleClicked()
{
    if (OwnerWidget)
    {
        OwnerWidget->OnUpgradeButtonClicked(this);
    }
}

// ---------- UShopWidget ----------
void UShopWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        AbilityManager = Player->FindComponentByClass<UAbilityManagerComponent>();
    }

    if (ButtonClose)
    {
        ButtonClose->OnClicked.AddDynamic(this, &UShopWidget::OnCloseClicked);
    }

    PopulateShop();
}

void UShopWidget::OnCloseClicked()
{
    SetVisibility(ESlateVisibility::Hidden);
}

void UShopWidget::PopulateShop()
{
    if (!AbilityManager || !ShopListBox)
        return;

    ShopListBox->ClearChildren();

    for (const FAbilitySpec& Spec : AbilityManager->EquippedAbilities)
    {
        if (!Spec.Ability) continue;
        UAbilityData* Ability = Spec.Ability;

        // --- Carte de l’ability ---
        UBorder* AbilityCard = NewObject<UBorder>(this);
        AbilityCard->SetPadding(FMargin(8));
        AbilityCard->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.35f));
        ShopListBox->AddChild(AbilityCard);

        UVerticalBox* EntryBox = NewObject<UVerticalBox>(this);
        AbilityCard->SetContent(EntryBox);

        // --- Icône + Nom ---
        UHorizontalBox* HeaderBox = NewObject<UHorizontalBox>(this);
        EntryBox->AddChildToVerticalBox(HeaderBox);

        if (Ability->Icon)
        {
            UImage* Icon = NewObject<UImage>(this);
            Icon->SetBrushFromTexture(Ability->Icon);
            Icon->SetDesiredSizeOverride(FVector2D(48.f, 48.f));

            UHorizontalBoxSlot* IconSlot = HeaderBox->AddChildToHorizontalBox(Icon);
            if (IconSlot)
            {
                IconSlot->SetPadding(FMargin(0, 0, 8, 0));
                IconSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
            }
        }

        UTextBlock* Title = NewObject<UTextBlock>(this);
        Title->SetText(Ability->AbilityName);
        Title->SetColorAndOpacity(FSlateColor(FLinearColor::White));
        HeaderBox->AddChildToHorizontalBox(Title);

        // --- Description ---
        if (!Ability->Description.IsEmpty())
        {
            UTextBlock* Desc = NewObject<UTextBlock>(this);
            Desc->SetText(Ability->Description);
            Desc->SetAutoWrapText(true);
            Desc->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
            EntryBox->AddChildToVerticalBox(Desc);
        }

        // --- Label Upgrades ---
        UTextBlock* Label = NewObject<UTextBlock>(this);
        Label->SetText(FText::FromString(TEXT("Available Upgrades:")));
        EntryBox->AddChildToVerticalBox(Label);

        const FAbilityUpgradeSet& Upgrades = Ability->BaseUpgrades;
        int32 NumUpgrades = 0;

        // --- Créateur de bouton ---
        auto MakeUpgradeButton = [&](UAbilityData* InAbility, const FString& Name, const FUpgradeStat& Stat)
            {
                if (Stat.UpgradeCosts.Num() == 0) return;
                NumUpgrades++;

                UShopUpgradeButton* Button = NewObject<UShopUpgradeButton>(this);
                Button->Ability = InAbility;
                Button->UpgradeName = Name;
                Button->OwnerWidget = this;

                // On récupère le niveau actuel depuis le manager
                int32 CurrentLevel = 0;
                if (const FUpgradeLevels* Levels = AbilityManager->UpgradeLevelsByAbility.Find(InAbility))
                {
                    if (Name == "Damage")           CurrentLevel = Levels->DamageLevel;
                    else if (Name == "Cooldown")         CurrentLevel = Levels->CooldownLevel;
                    else if (Name == "MultishotChance")  CurrentLevel = Levels->MultishotChanceLevel;
                    else if (Name == "MultishotAmount")  CurrentLevel = Levels->MultishotAmountLevel;
                    else if (Name == "BounceChance")     CurrentLevel = Levels->BounceChanceLevel;
                    else if (Name == "BounceAmount")     CurrentLevel = Levels->BounceAmountLevel;
                    else if (Name == "FrenzyChance")     CurrentLevel = Levels->FrenzyChanceLevel;
                }

                UTextBlock* BtnText = NewObject<UTextBlock>(this);

                // Si max level -> message spécifique
                if (CurrentLevel >= Stat.EffectValues.Num())
                {
                    BtnText->SetText(FText::FromString(FString::Printf(TEXT("%s: MAX (Lvl %d)"), *Name, CurrentLevel)));
                    Button->SetIsEnabled(false);
                }
                else
                {
                    // Valeur et coût du prochain niveau
                    const float Value = Stat.EffectValues.IsValidIndex(CurrentLevel) ? Stat.EffectValues[CurrentLevel] : Stat.EffectValues.Last();
                    const float Cost = Stat.UpgradeCosts.IsValidIndex(CurrentLevel) ? Stat.UpgradeCosts[CurrentLevel] : Stat.UpgradeCosts.Last();

                    BtnText->SetText(FText::FromString(
                        FString::Printf(TEXT("%s Lvl %d -> +%.1f (Cost %.0f)"), *Name, CurrentLevel, Value, Cost)
                    ));
                }

                BtnText->SetJustification(ETextJustify::Center);
                Button->SetContent(BtnText);
                Button->SetBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 1.f));
                Button->OnClicked.AddDynamic(Button, &UShopUpgradeButton::HandleClicked);

                EntryBox->AddChildToVerticalBox(Button);
            };


        // --- Crée les boutons ---
        MakeUpgradeButton(Ability, TEXT("Damage"), Upgrades.Damage);
        MakeUpgradeButton(Ability, TEXT("Cooldown"), Upgrades.Cooldown);
        MakeUpgradeButton(Ability, TEXT("MultishotChance"), Upgrades.MultishotChance);
        MakeUpgradeButton(Ability, TEXT("MultishotAmount"), Upgrades.MultishotAmount);
        MakeUpgradeButton(Ability, TEXT("BounceChance"), Upgrades.BounceChance);
        MakeUpgradeButton(Ability, TEXT("BounceAmount"), Upgrades.BounceAmount);
        MakeUpgradeButton(Ability, TEXT("FrenzyChance"), Upgrades.FrenzyChance);

        if (NumUpgrades == 0)
        {
            UTextBlock* NoUpgrade = NewObject<UTextBlock>(this);
            NoUpgrade->SetText(FText::FromString(TEXT("No upgrades available")));
            NoUpgrade->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
            EntryBox->AddChildToVerticalBox(NoUpgrade);
        }

        // --- Espacement ---
        UTextBlock* Spacer = NewObject<UTextBlock>(this);
        Spacer->SetText(FText::GetEmpty());
        Spacer->SetMargin(FMargin(0, 0, 0, 10));
        ShopListBox->AddChild(Spacer);
    }
}



void UShopWidget::OnUpgradeButtonClicked(UShopUpgradeButton* Btn)
{
    if (!Btn || !Btn->Ability || !AbilityManager) return;

    AbilityManager->UpgradeAbility(Btn->Ability, Btn->UpgradeName);

    UE_LOG(LogTemp, Log, TEXT("[SHOP] %s -> %s upgraded"),
        *Btn->Ability->AbilityName.ToString(),
        *Btn->UpgradeName);

    PopulateShop(); // refresh
}