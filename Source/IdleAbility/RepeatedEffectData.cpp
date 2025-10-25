 // Fill out your copyright notice in the Description page of Project Settings.

#include "RepeatedEffectData.h"
#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"
#include "Engine/World.h"

bool URepeatedEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    // Sécurité basique
    if (!Context.Source || !EffectToRepeat)
    {
        UE_LOG(LogTemp, Warning, TEXT("[RepeatedEffect] Contexte invalide ou EffectToRepeat manquant."));
        // On retourne true pour que, si Duration > 0, le système persistant puisse gérer l'expiration proprement.
        return true;
    }

    // Optionnel : warning design-time si RepeatCount saisi mais Duration non cohérente
    if (RepeatCount > 0 && TickInterval > 0.f)
    {
        const float Expected = RepeatCount * TickInterval;
        if (FMath::IsNearlyZero(Duration) || Duration + KINDA_SMALL_NUMBER < Expected)
        {
            UE_LOG(LogTemp, Warning, TEXT("[RepeatedEffect] RepeatCount=%d, TickInterval=%.3f -> pensez à mettre Duration ~ %.3f"),
                RepeatCount, TickInterval, Expected);
        }
    }

    // Cible morte : on peut ignorer l'exécution selon le paramètre
    if (Context.Target && !bIgnoreTargetDeath && !Context.Target->IsAlive())
    {
        // On ne lance pas l'effet, mais on laisse l'effet persistant vivre jusqu'à expiration (Duration)
        return true;
    }

    // Récupère le manager pour appliquer le sous-effet (traite persistant/instantané selon son type)
    UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager)
    {
        UE_LOG(LogTemp, Warning, TEXT("[RepeatedEffect] AbilityManager manquant sur %s"), *Context.Source->GetName());
        // Fallback : exécuter directement (sans persistance) pour ne pas hard-fail
        EffectToRepeat->ApplyEffect(Context);
        return true;
    }

    // Important : on passe par le manager pour bénéficier du pipeline (persistant, sous-effets, OnExpire, etc.)
    Manager->ApplyEffectToTarget(EffectToRepeat, Context);

    // On retourne true pour laisser le système persistant continuer jusqu’à expiration (Duration)
    return true;
}
