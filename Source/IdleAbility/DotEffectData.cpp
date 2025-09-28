// Fill out your copyright notice in the Description page of Project Settings.

#include "DotEffectData.h"
#include "CustomCharacter.h"
#include "AbilityData.h"

bool UDotEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Source || !Context.Target || !Context.Target->IsAlive())
        return true;

    const float dmg = Context.Source->Attack * DamagePerTickMultiplier + FlatDamagePerTick;

    // Type de dégâts = celui de l’ability (cohérent avec DamageEffect)
    const EDamageType dmgType = Context.Ability ? Context.Ability->AbilityType : EDamageType::Pure;

    Context.Target->TakeCustomDamage(dmg, dmgType, Context.Source);

    UE_LOG(LogTemp, Log, TEXT("[DOT] %s inflige %.1f à %s (Ability=%s)"),
        *Context.Source->GetName(),
        dmg,
        *Context.Target->GetName(),
        Context.Ability ? *Context.Ability->AbilityName.ToString() : TEXT("null"));

    return true;
}
