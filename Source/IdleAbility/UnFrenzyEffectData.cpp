// Fill out your copyright notice in the Description page of Project Settings.

#include "UnFrenzyEffectData.h"
#include "AbilityManagerComponent.h"
#include "AbilityData.h"
#include "CustomCharacter.h"

bool UUnFrenzyEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Source || !Context.Ability) return false;

    UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager) return false;

    FAbilitySpec* Spec = Manager->EquippedAbilities.FindByPredicate(
        [&](const FAbilitySpec& S) { return S.Ability == Context.Ability; });

    if (Spec)
    {
        Spec->CooldownScalar *= RevertScalar;

        UE_LOG(LogTemp, Warning, TEXT("[Frenzy] %s -> Ability %s sort de Frenzy (Scalar=%.2f)"),
            *Context.Source->GetName(),
            *Context.Ability->AbilityName.ToString(),
            Spec->CooldownScalar);
        return true;
    }

    return false;
}
