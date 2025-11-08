// Fill out your copyright notice in the Description page of Project Settings.

#include "AOEEffectData.h"
#include "CustomCharacter.h"
#include "AbilityManagerComponent.h"
#include "BaseProjectile.h"
#include "EngineUtils.h"

bool UAOEEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Source || !Context.Ability)
        return false;

    FVector Center;

    if (Context.Projectile)
    {
        Center = Context.Projectile->GetActorLocation();
    }
    else if (TriggerPhase == EEffectTriggerPhase::OnCast && Context.Source)
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

    TArray<ACustomCharacter*> Affected;
    for (TActorIterator<ACustomCharacter> It(World); It; ++It)
    {
        ACustomCharacter* C = *It;
        if (!C || !C->IsAlive()) continue;

        const float DistSq = FVector::DistSquared(C->GetActorLocation(), Center);
        if (DistSq <= Radius * Radius)
        {
            Affected.Add(C);
        }
    }

    // applique sous effet a tous ceux dans l'aoe, aka dans le radius
    for (ACustomCharacter* C : Affected)
    {
        FAbilityEffectContext NewCtx = Context;
        NewCtx.Target = C;

        for (UAbilityEffectData* Sub : SubEffects)
        {
            if (!Sub) continue;

            if (UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>())
            {
                Manager->ApplyEffectToTarget(Sub, NewCtx);
            }
            else
            {
                Sub->ApplyEffect(NewCtx);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[AOE] %s a affecte %d cibles (rayon=%.0f)"),
        *Context.Source->GetName(),
        Affected.Num(),
        Radius);

    return true;
}
