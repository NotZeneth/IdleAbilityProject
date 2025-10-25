// Fill out your copyright notice in the Description page of Project Settings.


#include "RetriggerEffectData.h"
#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"

bool URetriggerEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!EffectToRetrigger || !Context.Source)
        return false;

    // --- Roll de chance ---
    const float Roll = FMath::FRand();
    if (Roll > RetriggerChance)
    {
        UE_LOG(LogTemp, Log, TEXT("[Retrigger] %s roll=%.2f > chance=%.2f -> pas de retrigger"),
            *Context.Source->GetName(), Roll, RetriggerChance);
        return false;
    }

    // --- Récupération du manager ---
    UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager)
        return false;

    // --- Compteur de retriggers ---
    int32& Count = Manager->RetriggerCounts.FindOrAdd(this);

    if (Count >= MaxRetriggerCount)
    {
        UE_LOG(LogTemp, Log, TEXT("[Retrigger] %s a atteint la limite (%d) pour %s"),
            *Context.Source->GetName(), MaxRetriggerCount, *GetName());

        // Reset pour réutiliser plus tard
        Manager->RetriggerCounts.Remove(this);
        return false;
    }

    Count++;

    UE_LOG(LogTemp, Warning, TEXT("[Retrigger] %s relance %s (%d/%d)"),
        *Context.Source->GetName(),
        *EffectToRetrigger->GetName(),
        Count,
        MaxRetriggerCount);

    // --- Relance immédiate de l'effet ---
    Manager->ApplyEffectToTarget(EffectToRetrigger, Context);

    // Nettoyage auto si on a atteint la limite
    if (Count >= MaxRetriggerCount)
    {
        Manager->RetriggerCounts.Remove(this);
    }

    return true;
}
