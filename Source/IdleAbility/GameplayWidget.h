// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UEditableTextBox;
class UButton;
class APlayerCharacter;
class AWaveGameMode;
class UShopWidget;

UCLASS()
class IDLEABILITY_API UGameplayWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY()
    APlayerCharacter* PlayerRef = nullptr;
    AWaveGameMode* GameModeRef = nullptr;

    // --- Widgets liés ---
    UPROPERTY(meta = (BindWidget)) UProgressBar* HealthBar;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TextHealth;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TextGold;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TextGem;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TextWave;
    UPROPERTY(meta = (BindWidget)) UEditableTextBox* InputWave;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonAddGold;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonAddGem;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonJumpWave;
    UPROPERTY(meta = (BindWidget)) UButton* ButtonShop;




    // --- UI update API ---
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateHealth(float CurrentHP, float MaxHP);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateGold(int Gold);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateGem(int Gem);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateWave(int Wave);

    UFUNCTION()
    void OnToggleShop();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UShopWidget> ShopWidgetClass;

    UPROPERTY()
    UShopWidget* ShopWidgetRef = nullptr;




protected:
    // --- Boutons ---
    UFUNCTION() void OnAddGold();
    UFUNCTION() void OnAddGem();
    UFUNCTION() void OnJumpWave();
};
