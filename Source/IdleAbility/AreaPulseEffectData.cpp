// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaPulseEffectData.h"
#include "AbilityManagerComponent.h"
#include "EnemyCharacter.h"
#include "CustomCharacter.h"
#include "WaveGameMode.h"
#include "BaseProjectile.h"

bool UAreaPulseEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!InnerEffect || !Context.Source)
        return true;

    const FVector Center =
        (Context.Projectile ? Context.Projectile->GetActorLocation()
            : (Context.Target ? Context.Target->GetActorLocation()
                : Context.Source->GetActorLocation()));

    UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager || !Manager->GameModeRef)
        return true;

    const float RadiusSq = Radius * Radius;

    TArray<AEnemyCharacter*> EnemiesSnapshot = Manager->GameModeRef->EnemyList;

    for (AEnemyCharacter* Enemy : EnemiesSnapshot)
    {
        if (!Enemy || !Enemy->IsAlive()) continue;

        const float DistSq = FVector::DistSquared(Enemy->GetActorLocation(), Center);
        if (DistSq > RadiusSq) continue;

        FAbilityEffectContext HitCtx = Context;
        HitCtx.Target = Enemy;

        InnerEffect->ApplyEffect(HitCtx);
    }

    return true;
}

