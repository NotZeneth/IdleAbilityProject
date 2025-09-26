// Fill out your copyright notice in the Description page of Project Settings.

#include "DestroyProjectileEffectData.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.h"

void UDestroyProjectileEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (Context.Projectile)
    {
        Context.Projectile->Destroy();
        UE_LOG(LogTemp, Warning, TEXT("[DestroyOnHit] Projectile détruit après impact."));
    }
}
