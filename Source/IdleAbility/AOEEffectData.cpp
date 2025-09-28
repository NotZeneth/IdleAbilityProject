// Fill out your copyright notice in the Description page of Project Settings.

#include "AOEEffectData.h"
#include "CustomCharacter.h"
#include "AbilityManagerComponent.h"
#include "EngineUtils.h"

bool UAOEEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Source || !Context.Ability)
        return false;

    // centre de l'AOE
    FVector Center;
    if (TriggerPhase == EEffectTriggerPhase::OnCast && Context.Source)
    {
        Center = Context.Source->GetActorLocation();
    }
    else if (TriggerPhase == EEffectTriggerPhase::OnHit && Context.Target)
    {
        Center = Context.Target->GetActorLocation();
    }
    else
    {
        return false;
    }

    UWorld* World = Context.Source->GetWorld();
    if (!World) return false;

    // récupérer toutes les cibles potentielles
    TArray<ACustomCharacter*> Affected;
    for (TActorIterator<ACustomCharacter> It(World); It; ++It)
    {
        ACustomCharacter* C = *It;
        if (!C || !C->IsAlive()) continue;

        float DistSq = FVector::DistSquared(C->GetActorLocation(), Center);
        if (DistSq <= Radius * Radius)
        {
            Affected.Add(C);
        }
    }

    // appliquer tous les SubEffects sur chaque cible
    for (ACustomCharacter* C : Affected)
    {
        FAbilityEffectContext NewCtx = Context;
        NewCtx.Target = C;

        for (UAbilityEffectData* Sub : SubEffects)
        {
            if (Sub)
            {
                // persistant géré par le Manager
                UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
                if (Manager)
                    Manager->ApplyEffectToTarget(Sub, NewCtx);
                else
                    Sub->ApplyEffect(NewCtx);
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[AOE] %s a affecté %d cibles (rayon=%.0f)"),
        *Context.Source->GetName(),
        Affected.Num(),
        Radius);

    return true;
}
