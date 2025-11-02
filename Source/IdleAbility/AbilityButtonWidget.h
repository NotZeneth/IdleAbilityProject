// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityButtonWidget.generated.h"

class UButton;
class UTextBlock;
class UAbilityManagerComponent;
class UImage;

UCLASS()
class IDLEABILITY_API UAbilityButtonWidget : public UUserWidget
{
    GENERATED_BODY()

public:
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
    void OnRightClicked();

private:
    UPROPERTY(meta = (BindWidgetOptional))
    UButton* Button_Ability;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* Text_Cooldown;

    UPROPERTY(meta = (BindWidgetOptional))
    UImage* AbilityIcon; // oui je peux ecrire class UImage mais bon whatever
};
