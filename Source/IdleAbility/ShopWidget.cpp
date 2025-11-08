// Fill out your copyright notice in the Description page of Project Settings.

#include "ShopWidget.h"

#include "AbilityData.h"
#include "AbilityManagerComponent.h"
#include "PlayerCharacter.h"
#include "GameplayWidget.h"
#include "AbilityBarWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UShopUpgradeButton::HandleClicked()
{
    if (OwnerWidget)
    {
        OwnerWidget->OnUpgradeButtonClicked(this);
    }
}

void UPlayerUpgradeButton::HandleClicked()
{
    if (OwnerWidget)
    {
        OwnerWidget->OnPlayerUpgradeClicked(StatName);
    }
}
void UUnlockAbilityButton::HandleClicked()
{
    if (OwnerWidget)
    {
        OwnerWidget->OnUnlockAbilityClicked(Ability);
    }
}


void UShopWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        AbilityManager = Player->FindComponentByClass<UAbilityManagerComponent>();
    }

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->SetPause(true);
    }

    if (ButtonClose)
    {
        ButtonClose->OnClicked.AddDynamic(this, &UShopWidget::OnCloseClicked);
    }

    PopulateShop();
}

void UShopWidget::OnCloseClicked()
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->SetPause(false);
        PC->bShowMouseCursor = false;
    }

    SetVisibility(ESlateVisibility::Hidden);
}

void UShopWidget::PopulateShop()
{
    if (!AbilityManager || !ShopListBox)
        return;

    ShopListBox->ClearChildren();

    PopulatePlayerUpgrades();

    for (const FAbilitySpec& Spec : AbilityManager->EquippedAbilities)
    {
        if (!Spec.Ability) continue;
        UAbilityData* Ability = Spec.Ability;

        UBorder* AbilityCard = NewObject<UBorder>(this);
        AbilityCard->SetPadding(FMargin(8));
        AbilityCard->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.35f));
        ShopListBox->AddChild(AbilityCard);

        UVerticalBox* EntryBox = NewObject<UVerticalBox>(this);
        AbilityCard->SetContent(EntryBox);

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

        if (!Ability->Description.IsEmpty())
        {
            UTextBlock* Desc = NewObject<UTextBlock>(this);
            Desc->SetText(Ability->Description);
            Desc->SetAutoWrapText(true);
            Desc->SetColorAndOpacity(FSlateColor(FLinearColor(0.8f, 0.8f, 0.8f)));
            EntryBox->AddChildToVerticalBox(Desc);
        }

      
        if (!Spec.bUnlocked)
        {
            const int32 Cost = Ability->GemCostToUnlock;

            UUnlockAbilityButton* UnlockBtn = NewObject<UUnlockAbilityButton>(this);
            UnlockBtn->OwnerWidget = this;
            UnlockBtn->Ability = Ability;
            UnlockBtn->OnClicked.AddDynamic(UnlockBtn, &UUnlockAbilityButton::HandleClicked);
            UnlockBtn->SetBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 1.f));

            UTextBlock* UnlockText = NewObject<UTextBlock>(this);
            UnlockText->SetJustification(ETextJustify::Center);
            UnlockText->SetText(FText::FromString(FString::Printf(TEXT("Unlock for %d Gems"), Cost)));
            UnlockBtn->SetContent(UnlockText);

            
            if (AbilityManager->PlayerRef)
            {
                const bool bCanAfford = AbilityManager->PlayerRef->CurrentGem >= Cost;
                UnlockBtn->SetIsEnabled(bCanAfford);
            }

            EntryBox->AddChildToVerticalBox(UnlockBtn);

            UTextBlock* Spacer = NewObject<UTextBlock>(this);
            Spacer->SetText(FText::GetEmpty());
            Spacer->SetMargin(FMargin(0, 0, 0, 10));
            ShopListBox->AddChild(Spacer);

            continue;
        }

       
        UTextBlock* Label = NewObject<UTextBlock>(this);
        Label->SetText(FText::FromString(TEXT("Available Upgrades:")));
        EntryBox->AddChildToVerticalBox(Label);

        const FAbilityUpgradeSet& Upgrades = Ability->BaseUpgrades;
        int32 NumUpgrades = 0;

        auto MakeUpgradeButton = [&](UAbilityData* InAbility, const FString& Name, const FUpgradeStat& Stat)
            {
                if (Stat.UpgradeCosts.Num() == 0) return;
                NumUpgrades++;

                UShopUpgradeButton* Button = NewObject<UShopUpgradeButton>(this);
                Button->Ability = InAbility;
                Button->UpgradeName = Name;
                Button->OwnerWidget = this;

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
                    else if (Name == "RetriggerChance")  CurrentLevel = Levels->RetriggerChanceLevel;

                }

                UTextBlock* BtnText = NewObject<UTextBlock>(this);

                // Max level ?
                if (CurrentLevel >= Stat.EffectValues.Num() - 1)
                {
                    const float ValueAtMax = Stat.EffectValues.IsValidIndex(CurrentLevel)
                        ? Stat.EffectValues[CurrentLevel]
                        : (Stat.EffectValues.Num() > 0 ? Stat.EffectValues.Last() : 0.f);

                    BtnText->SetText(FText::FromString(
                        FString::Printf(TEXT("[Lvl %d] %s : %.1f (MAX)"),
                            CurrentLevel + 1, *Name, ValueAtMax)));

                    Button->SetIsEnabled(false);
                }
                else
                {
                    const float Value = Stat.EffectValues.IsValidIndex(CurrentLevel)
                        ? Stat.EffectValues[CurrentLevel]
                        : (Stat.EffectValues.Num() > 0 ? Stat.EffectValues.Last() : 0.f);

                    const float Cost = Stat.UpgradeCosts.IsValidIndex(CurrentLevel)
                        ? Stat.UpgradeCosts[CurrentLevel]
                        : (Stat.UpgradeCosts.Num() > 0 ? Stat.UpgradeCosts.Last() : 0.f);

                    BtnText->SetText(FText::FromString(
                        FString::Printf(TEXT("[Lvl %d] %s : %.1f (Upgrade : %.0f gold)"),
                            CurrentLevel + 1, *Name, Value, Cost)));

                    Button->SetIsEnabled(true);
                }

                BtnText->SetJustification(ETextJustify::Center);
                Button->SetContent(BtnText);
                Button->SetBackgroundColor(FLinearColor(0.15f, 0.15f, 0.15f, 1.f));
                Button->OnClicked.AddDynamic(Button, &UShopUpgradeButton::HandleClicked);

                EntryBox->AddChildToVerticalBox(Button);
            };

        MakeUpgradeButton(Ability, TEXT("Damage"), Upgrades.Damage);
        MakeUpgradeButton(Ability, TEXT("Cooldown"), Upgrades.Cooldown);
        MakeUpgradeButton(Ability, TEXT("MultishotChance"), Upgrades.MultishotChance);
        MakeUpgradeButton(Ability, TEXT("MultishotAmount"), Upgrades.MultishotAmount);
        MakeUpgradeButton(Ability, TEXT("BounceChance"), Upgrades.BounceChance);
        MakeUpgradeButton(Ability, TEXT("BounceAmount"), Upgrades.BounceAmount);
        MakeUpgradeButton(Ability, TEXT("FrenzyChance"), Upgrades.FrenzyChance);
        MakeUpgradeButton(Ability, TEXT("RetriggerChance"), Upgrades.RetriggerChance);


        if (NumUpgrades == 0)
        {
            UTextBlock* NoUpgrade = NewObject<UTextBlock>(this);
            NoUpgrade->SetText(FText::FromString(TEXT("No upgrades available")));
            NoUpgrade->SetColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)));
            EntryBox->AddChildToVerticalBox(NoUpgrade);
        }

        UTextBlock* Spacer = NewObject<UTextBlock>(this);
        Spacer->SetText(FText::GetEmpty());
        Spacer->SetMargin(FMargin(0, 0, 0, 10));
        ShopListBox->AddChild(Spacer);
    }
}


void UShopWidget::PopulatePlayerUpgrades()
{
    if (!AbilityManager || !ShopListBox)
        return;

    UBorder* PlayerCard = NewObject<UBorder>(this);
    PlayerCard->SetPadding(FMargin(8));
    PlayerCard->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.35f));
    ShopListBox->AddChild(PlayerCard);

    UVerticalBox* EntryBox = NewObject<UVerticalBox>(this);
    PlayerCard->SetContent(EntryBox);

    UTextBlock* Header = NewObject<UTextBlock>(this);
    Header->SetText(FText::FromString(TEXT("PLAYER UPGRADES")));
    Header->SetJustification(ETextJustify::Center);
    Header->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.9f, 0.3f))); // doré
    {
        FSlateFontInfo Font = Header->GetFont();
        Font.Size = 20;
        Header->SetFont(Font);
    }
    EntryBox->AddChildToVerticalBox(Header);

    UTextBlock* SpacerTop = NewObject<UTextBlock>(this);
    SpacerTop->SetText(FText::GetEmpty());
    SpacerTop->SetMargin(FMargin(0, 0, 0, 5));
    EntryBox->AddChildToVerticalBox(SpacerTop);

    auto MakeButton = [&](const FString& StatName, const FString& Label)
        {
            FPlayerUpgrade* Stat = nullptr;
            if (StatName == "AttackFlat")    Stat = &AbilityManager->AttackFlat;
            else if (StatName == "MaxHPFlat")     Stat = &AbilityManager->MaxHPFlat;
            else if (StatName == "AttackPercent") Stat = &AbilityManager->AttackPercent;
            else if (StatName == "HPPercent")     Stat = &AbilityManager->HPPercent;
            if (!Stat) return;

            const int32 Lvl = Stat->Level;
            const float Val = Stat->GetCurrentValue();
            const float Cost = Stat->GetNextCost();

            UPlayerUpgradeButton* Button = NewObject<UPlayerUpgradeButton>(this);
            Button->OwnerWidget = this;
            Button->StatName = StatName;
            Button->OnClicked.AddDynamic(Button, &UPlayerUpgradeButton::HandleClicked);
            Button->SetBackgroundColor(FLinearColor(0.18f, 0.18f, 0.18f, 1.f));

            UHorizontalBox* InnerBox = NewObject<UHorizontalBox>(this);
            UHorizontalBoxSlot* TxtSlot = nullptr;


            UTextBlock* BtnText = NewObject<UTextBlock>(this);
            FString Line;

            if (StatName == "AttackPercent" || StatName == "HPPercent")
                Line = FString::Printf(TEXT("[Lvl %d] %s : +%d%% (Cost: %.0f gold)"), Lvl, *Label, Lvl, Cost);
            else
                Line = FString::Printf(TEXT("[Lvl %d] %s : %.0f (Cost: %.0f gold)"), Lvl, *Label, Val, Cost);

            BtnText->SetText(FText::FromString(Line));
            BtnText->SetJustification(ETextJustify::Center);
            BtnText->SetColorAndOpacity(FSlateColor(FLinearColor(0.95f, 0.95f, 0.95f)));

            {
                FSlateFontInfo Font = BtnText->GetFont();
                Font.Size = 14;
                BtnText->SetFont(Font);
            }

            Button->SetContent(BtnText);
            EntryBox->AddChildToVerticalBox(Button);

            UTextBlock* SmallSpacer = NewObject<UTextBlock>(this);
            SmallSpacer->SetText(FText::GetEmpty());
            SmallSpacer->SetMargin(FMargin(0, 0, 0, 4));
            EntryBox->AddChildToVerticalBox(SmallSpacer);
        };

    MakeButton("AttackFlat", "Attack");
    MakeButton("MaxHPFlat", "Max HP");
    MakeButton("AttackPercent", "Attack %");
    MakeButton("HPPercent", "HP %");

    UTextBlock* SpacerBottom = NewObject<UTextBlock>(this);
    SpacerBottom->SetText(FText::GetEmpty());
    SpacerBottom->SetMargin(FMargin(0, 0, 0, 10));
    ShopListBox->AddChild(SpacerBottom);
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

void UShopWidget::OnPlayerUpgradeClicked(const FString& StatName)
{
    if (!AbilityManager) return;

    if (AbilityManager->UpgradePlayerStat(StatName))
    {
        ShopListBox->ClearChildren();
        PopulatePlayerUpgrades();
        PopulateShop();
    }

    if (AbilityManager && AbilityManager->PlayerRef && AbilityManager->PlayerRef->GameplayWidgetRef)
    {
        UGameplayWidget* GameplayWidget = AbilityManager->PlayerRef->GameplayWidgetRef;

        if (AbilityManager->PlayerRef->AbilityWidgetRef)
        {
            AbilityManager->PlayerRef->AbilityWidgetRef->RefreshButtons();
        }
    }

}

void UShopWidget::OnUnlockAbilityClicked(UAbilityData* Ability)
{
    if (!Ability || !AbilityManager || !AbilityManager->PlayerRef) return;

    APlayerCharacter* Player = AbilityManager->PlayerRef;

    FAbilitySpec* FoundSpec = nullptr;
    for (FAbilitySpec& Spec : AbilityManager->EquippedAbilities)
    {
        if (Spec.Ability == Ability)
        {
            FoundSpec = &Spec;
            break;
        }
    }

    if (!FoundSpec)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SHOP] Ability non trouvée dans les specs du joueur"));
        return;
    }

    if (FoundSpec->bUnlocked)
        return;

    const int32 Cost = Ability->GemCostToUnlock;

    if (Player->CurrentGem >= Cost)
    {
        Player->AddGem(-Cost);
        FoundSpec->bUnlocked = true;

        UE_LOG(LogTemp, Log, TEXT("[SHOP] Unlocked %s for %d gems"),
            *Ability->AbilityName.ToString(), Cost);

        PopulateShop();

        if (Player->AbilityWidgetRef)
        {
            Player->AbilityWidgetRef->RefreshButtons();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[SHOP] Not enough gems to unlock %s (need %d)"),
            *Ability->AbilityName.ToString(), Cost);
    }
}

