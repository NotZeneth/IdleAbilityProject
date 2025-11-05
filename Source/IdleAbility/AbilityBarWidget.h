// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityBarWidget.generated.h"

class UHorizontalBox;
class UAbilityButtonWidget;
class UAbilityManagerComponent;

/**
 * Widget C++ représentant la barre d'abilities du joueur.
 * Il crée dynamiquement un bouton par ability équipée.
 * Chaque bouton gère ses clics gauche/droit et affiche le cooldown.
 */
UCLASS()
class IDLEABILITY_API UAbilityBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void RefreshButtons();

protected:
    /** Container horizontal des boutons */
    UPROPERTY(EditAnywhere, meta = (BindWidget))
    UHorizontalBox* ButtonContainer;

    /** Classe C++ du widget bouton individuel */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TSubclassOf<UAbilityButtonWidget> AbilityButtonClass;


    /** Référence vers le AbilityManager du joueur */
    UPROPERTY()
    UAbilityManagerComponent* AbilityManager;
};
