// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityBarWidget.generated.h"

class UHorizontalBox;
class UAbilityButtonWidget;
class UAbilityManagerComponent;

// Un widget juste pour la barre d'action du joueur, plutot que de faire un qui fait absolument tout je split

UCLASS()
class IDLEABILITY_API UAbilityBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // On refait les boutons, ca gere le cas ou les boutons sont modified, added etc
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void RefreshButtons();

protected:

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    UHorizontalBox* ButtonContainer;

    // La on a fait un widget specifique juste pour les boutons pour factoriser (approche normale mais ailleurs on a fait dif pour varier)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TSubclassOf<UAbilityButtonWidget> AbilityButtonClass;

    UPROPERTY()
    UAbilityManagerComponent* AbilityManager = nullptr;
};
