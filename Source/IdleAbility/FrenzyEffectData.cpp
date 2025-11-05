// Fill out your copyright notice in the Description page of Project Settings.

#include "FrenzyEffectData.h"
#include "AbilityManagerComponent.h"
#include "AbilityData.h"
#include "CustomCharacter.h"

bool UFrenzyEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Source || !Context.Ability) return false;

    UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager) return false;

    float& LastTime = Manager->LastFrenzyTimes.FindOrAdd(Context.Ability);
    float Now = Context.Source->GetWorld()->TimeSeconds;

    // Premier cast : pas de cooldown
    if (LastTime > 0.0f && Now - LastTime < InternalCooldown)
    {
        //UE_LOG(LogTemp, Warning, TEXT("[Frenzy] %s a lancé %s mais ICD pas fini (reste %.2fs)"),
        //    *Context.Source->GetName(),
        //    *Context.Ability->AbilityName.ToString(),
        //    InternalCooldown - (Now - LastTime));
        return false;
    }

    // Chance de proc
    float EffectiveChance = Context.Source->FrenzyChance * ChanceMultiplier;
    if (FMath::FRand() > EffectiveChance)
    {
        return false;
    }

    // Si proc -> modifier le cooldown scalar
    FAbilitySpec* Spec = Manager->EquippedAbilities.FindByPredicate([&](const FAbilitySpec& S) {
        return S.Ability == Context.Ability;
        });

    if (Spec)
    {
        Spec->CooldownScalar *= FrenzyScalar;
        LastTime = Now;
        return true;
    }

    return false;
}
