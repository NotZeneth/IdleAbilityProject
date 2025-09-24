// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageEffect.h"
#include "CustomCharacter.h"
#include "AbilityData.h"

void UDamageEffect::ApplyEffect_Implementation(const FAbilityEffectContext& Context)
{
    if (!Context.Target || !Context.AbilityData) return;

    float Dmg = Context.AbilityData->PrimaryValue;
    Context.Target->TakeCustomDamage(Dmg, Context.AbilityData->AbilityType, Context.Source);

    UE_LOG(LogTemp, Warning, TEXT("%s inflige %.1f dégâts (%s) à %s"),
        *Context.Source->GetName(),
        Dmg,
        *UEnum::GetValueAsString(Context.AbilityData->AbilityType),
        *Context.Target->GetName());
}
