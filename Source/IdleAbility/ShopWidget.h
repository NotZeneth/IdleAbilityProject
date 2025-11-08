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

// Classe abominable : pour les boutons d'ability j'ai fait tout gentillement une horizontal box et un widget bouton d'ability
// La je voulais essayer de faire ca en full c++ : bah heu wai ca marche mais a quel prix ?
// Mais j'ai appris des trucs et ca c'est précieux :)

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
class IDLEABILITY_API UPlayerUpgradeButton : public UButton
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FString StatName;

    UPROPERTY()
    class UShopWidget* OwnerWidget = nullptr;

    UFUNCTION()
    void HandleClicked();
};

// unlock 
UCLASS()
class IDLEABILITY_API UUnlockAbilityButton : public UButton
{
    GENERATED_BODY()
public:
    UPROPERTY() class UAbilityData* Ability = nullptr;
    UPROPERTY() class UShopWidget* OwnerWidget = nullptr;

    UFUNCTION() void HandleClicked();
};


UCLASS()
class IDLEABILITY_API UShopWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void PopulateShop();

    UFUNCTION(BlueprintCallable)
    void PopulatePlayerUpgrades();

    void OnUpgradeButtonClicked(UShopUpgradeButton* Btn);

    UFUNCTION()
    void OnPlayerUpgradeClicked(const FString& StatName);

    UFUNCTION()
    void OnUnlockAbilityClicked(class UAbilityData* Ability);




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
