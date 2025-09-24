// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageEffect.h"
#include "CustomCharacter.h"
#include "AbilityData.h"

void UDamageEffect::ApplyEffect_Implementation(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData)
{
    if (!Target || !AbilityData) return;

    float dmg = AbilityData->PrimaryValue;
    Target->TakeCustomDamage(dmg, AbilityData->AbilityType, Source);

    for (UAbilityEffect* SubEffect : SubEffects)
    {
        if (SubEffect)
        {
            SubEffect->ApplyEffect(Source, Target, AbilityData);
        }
    }

}
