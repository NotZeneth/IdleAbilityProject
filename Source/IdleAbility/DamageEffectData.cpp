// Fill out your copyright notice in the Description page of Project Settings.

#include "DamageEffectData.h"
#include "CustomCharacter.h"
#include "AbilityManagerComponent.h"
#include "AbilityData.h"
#include "PlayerCharacter.h"

bool UDamageEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Source || !Context.Target || !Context.Target->IsAlive())
        return true;

    // valeur par défaut depuis l’asset
    float Mult = DamageMultiplier;

    // override par l’upgrade si dispo
    if (Context.Ability)
    {
        if (UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>())
        {
            const float UpgradeMult = Manager->GetUpgradeValue(Context.Ability, TEXT("Damage"));
            if (UpgradeMult > 0.f)
            {
                Mult = UpgradeMult;
            }
        }
    }

    // attaque du joueur (si PlayerCharacter, utilise GetPlayerAttack)
    float AttackValue = Context.Source->Attack;
    if (APlayerCharacter* PC = Cast<APlayerCharacter>(Context.Source))
    {
        AttackValue = PC->GetPlayerAttack();
    }

    const float FinalDamage = AttackValue * Mult;

    Context.Target->TakeCustomDamage(FinalDamage, Context.Ability->AbilityType, Context.Source);

    UE_LOG(LogTemp, Warning, TEXT("[DamageEffect] %s inflige %.1f dégâts à %s via %s (Mult=%.2f)"),
        *Context.Source->GetName(),
        FinalDamage,
        *Context.Target->GetName(),
        Context.Ability ? *Context.Ability->AbilityName.ToString() : TEXT("NoAbility"),
        Mult);

    return true;
}
