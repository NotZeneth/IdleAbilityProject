// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ShopWidget.generated.h"

class UTextBlock;
class UScrollBox;
class UVerticalBox;
class UImage;
class UBorder;
class UAbilityManagerComponent;
class UAbilityData;

UCLASS()
class IDLEABILITY_API UShopUpgradeButton : public UButton
{
    GENERATED_BODY()
public:
    UPROPERTY() UAbilityData* Ability = nullptr;
    UPROPERTY() FString UpgradeName;
    UPROPERTY() class UShopWidget* OwnerWidget = nullptr;

    UFUNCTION()
    void HandleClicked();
};

UCLASS()
class IDLEABILITY_API UShopWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void PopulateShop();

    // Appelé par le bouton custom
    void OnUpgradeButtonClicked(UShopUpgradeButton* Btn);

protected:
    UPROPERTY(meta = (BindWidget))
    UScrollBox* ShopListBox;

    UPROPERTY(meta = (BindWidget))
    UButton* ButtonClose;

    UFUNCTION()
    void OnCloseClicked();

    UPROPERTY()
    UAbilityManagerComponent* AbilityManager = nullptr;
};
