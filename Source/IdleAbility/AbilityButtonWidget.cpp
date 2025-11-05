// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityButtonWidget.h"
#include "AbilityManagerComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "PlayerCharacter.h"


void UAbilityButtonWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!AbilityManager)
    {
        if (APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
            AbilityManager = Player->FindComponentByClass<UAbilityManagerComponent>();
    }

    if (Button_Ability)
    {
        Button_Ability->OnClicked.AddDynamic(this, &UAbilityButtonWidget::OnLeftClicked);
    }

    if (Button_AutoCast)
    {
        Button_AutoCast->OnClicked.AddDynamic(this, &UAbilityButtonWidget::OnToggleAutoCast);
    }

    // l'icone
    if (AbilityManager && AbilityManager->EquippedAbilities.IsValidIndex(AbilityIndex))
    {
        const FAbilitySpec& Spec = AbilityManager->EquippedAbilities[AbilityIndex];
        if (Spec.Ability && Spec.Ability->Icon && AbilityIcon)
        {
            AbilityIcon->SetBrushFromTexture(Spec.Ability->Icon);
        }
    }
}


void UAbilityButtonWidget::OnLeftClicked()
{
    if (!AbilityManager) return;
    if (!AbilityManager->EquippedAbilities.IsValidIndex(AbilityIndex)) return;  // oui bon c'est overkill mais bon j'avais envie de test la fonction

    AbilityManager->TryActivateAbility(AbilityIndex);
}

void UAbilityButtonWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!AbilityManager || !AbilityManager->EquippedAbilities.IsValidIndex(AbilityIndex))
        return;

    const FAbilitySpec& Spec = AbilityManager->EquippedAbilities[AbilityIndex];
    if (!Spec.Ability) return;

    float Remaining = FMath::Max(0.f, Spec.CooldownEndTime - GetWorld()->TimeSeconds);
    if (Text_Cooldown)
    {
        if (Remaining > 0.f)
            Text_Cooldown->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), Remaining)));
        else
            Text_Cooldown->SetText(FText::GetEmpty());
    }
}

void UAbilityButtonWidget::OnToggleAutoCast()
{
    if (!AbilityManager) return;
    if (!AbilityManager->EquippedAbilities.IsValidIndex(AbilityIndex)) return;

    FAbilitySpec& Spec = AbilityManager->EquippedAbilities[AbilityIndex];
    Spec.isAutoCast = !Spec.isAutoCast;

    UE_LOG(LogTemp, Warning, TEXT("[UI] Ability %d autocast = %s"),
        AbilityIndex, Spec.isAutoCast ? TEXT("ON") : TEXT("OFF"));
}
