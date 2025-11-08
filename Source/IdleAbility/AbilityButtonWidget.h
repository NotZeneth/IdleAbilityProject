// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityButtonWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UAbilityManagerComponent;

// Widget pour chaque bouton d'ability, y a un bouton duh et un bouton set auto en gros

UCLASS()
class IDLEABILITY_API UAbilityButtonWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // l'indexe dans le manager, car derriere on veut pouvoir recup des infos donc pratique
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    int32 AbilityIndex = 0;

    UPROPERTY()
    UAbilityManagerComponent* AbilityManager = nullptr;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION()
    void OnLeftClicked();

    UFUNCTION()
    void OnToggleAutoCast();

private:
    UPROPERTY(EditAnywhere, meta = (BindWidget))
    UButton* Button_Ability;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    UTextBlock* Text_Cooldown;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    UImage* AbilityIcon;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    UButton* Button_AutoCast;
};
