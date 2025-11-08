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
    TornadoPos.Z = TargetPos.Z;

    FVector toCenter = TornadoPos - TargetPos;
    float dist = toCenter.Size();

    // Ca marche
    if (dist > MaxRange)
    {
        UE_LOG(LogTemp, Verbose, TEXT("[DragEffect] %s est hors de portée de la tornade (%.1f > %.1f)"),
            *Context.Target->GetName(), dist, MaxRange);
        return false; // ca arrête de tick
    }

    if (dist <= Strength)
    {
        Context.Target->SetActorLocation(TornadoPos);
        return true;
    }

    FVector dir = toCenter / dist;
    Context.Target->SetActorLocation(TargetPos + dir * Strength);
    return true;
}
