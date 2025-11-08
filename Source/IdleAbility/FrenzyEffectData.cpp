// Fill out your copyright notice in the Description page of Project Settings.

#include "FrenzyEffectData.h"
#include "AbilityManagerComponent.h"
#include "AbilityData.h"
#include "CustomCharacter.h"

bool UFrenzyEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Source || !Context.Ability)
        return false;

    UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager)
        return false;

    // --- Internal Cooldown (ICD) ---
    float& LastTime = Manager->LastFrenzyTimes.FindOrAdd(Context.Ability);
    const float Now = Context.Source->GetWorld()->TimeSeconds;

    if (LastTime > 0.0f && Now - LastTime < InternalCooldown)
    {
        return false;
    }

    // --- Déterminer la chance de proc ---
    float Chance = TriggerChance;

    // si un track d’upgrade "FrenzyChance" existe, on prend la valeur correspondante
    const FAbilityUpgradeSet& Up = Context.Ability->BaseUpgrades;
    if (Up.FrenzyChance.EffectValues.Num() > 0)
    {
        const float UpChance = Manager->GetUpgradeValue(Context.Ability, TEXT("FrenzyChance"));
        Chance = FMath::Clamp(UpChance, 0.f, 1.f);
    }

    // --- RNG ---
    if (FMath::FRand() > Chance)
    {
        return false;
    }

    // --- Appliquer le frenzy ---
    FAbilitySpec* Spec = Manager->EquippedAbilities.FindByPredicate(
        [&](const FAbilitySpec& S) { return S.Ability == Context.Ability; });

    if (Spec)
    {
        Spec->CooldownScalar *= FrenzyScalar; // réduction du cooldown (temporaire)
        LastTime = Now;

        UE_LOG(LogTemp, Warning, TEXT("[Frenzy] Proc sur %s : chance=%.2f, scalar=%.2f"),
            *Context.Ability->AbilityName.ToString(), Chance, FrenzyScalar);

        return true;
    }

    return false;
}
