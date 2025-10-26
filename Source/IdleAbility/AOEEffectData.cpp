// Fill out your copyright notice in the Description page of Project Settings.

#include "AOEEffectData.h"
#include "CustomCharacter.h"
#include "BaseProjectile.h"
#include "AbilityManagerComponent.h"
#include "EngineUtils.h"

bool UAOEEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Source || !Context.Ability)
        return false;

    // 1) Centre prioritaire: le projectile s'il existe (spike, tornade, zone persistante)
    FVector Center;
    if (Context.Projectile)
    {
        Center = Context.Projectile->GetActorLocation();
    }
    // 2) Sinon, comportement historique
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

    // Rassembler toutes les cibles dans le rayon
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

    // Appliquer les SubEffects a chaque cible trouvee
    for (ACustomCharacter* C : Affected)
    {
        FAbilityEffectContext NewCtx = Context;
        NewCtx.Target = C;

        for (UAbilityEffectData* Sub : SubEffects)
        {
            if (!Sub) continue;

            UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
            if (Manager)
                Manager->ApplyEffectToTarget(Sub, NewCtx);
            else
                Sub->ApplyEffect(NewCtx);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[AOE] %s a affecte %d cibles (rayon=%.0f)"),
        *Context.Source->GetName(),
        Affected.Num(),
        Radius);

    return true;
}
