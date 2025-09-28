// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageEffectData.h"
#include "CustomCharacter.h"
#include "AbilityData.h"

bool UDamageEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Source || !Context.Target || !Context.Target->IsAlive())
        return true;

    // Dégâts = attaque du joueur × multiplicateur
    float FinalDamage = Context.Source->Attack * DamageMultiplier;

    Context.Target->TakeCustomDamage(FinalDamage, Context.Ability->AbilityType, Context.Source);

    UE_LOG(LogTemp, Warning, TEXT("[DamageEffect] %s inflige %.1f dégâts à %s via %s"),
        *Context.Source->GetName(),
        FinalDamage,
        *Context.Target->GetName(),
        *Context.Ability->AbilityName.ToString());

    return true;
}
