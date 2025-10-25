// Fill out your copyright notice in the Description page of Project Settings.


#include "DragEffectData.h"
#include "CustomCharacter.h"
#include "BaseProjectile.h"

bool UDragEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!Context.Target || !Context.Projectile)
        return false;

    FVector TornadoPos = Context.Projectile->GetActorLocation();
    FVector TargetPos = Context.Target->GetActorLocation();

    // on garde l'altitude du mob
    TornadoPos.Z = TargetPos.Z;

    FVector toCenter = TornadoPos - TargetPos;
    float dist = toCenter.Size();

    // éviter oscillations (overshoot)
    if (dist <= Strength)
    {
        Context.Target->SetActorLocation(TornadoPos);
        return true;
    }

    FVector dir = toCenter / dist;
    Context.Target->SetActorLocation(TargetPos + dir * Strength);

    return true;
}
