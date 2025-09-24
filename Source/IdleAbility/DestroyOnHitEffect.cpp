// Fill out your copyright notice in the Description page of Project Settings.


#include "DestroyOnHitEffect.h"
#include "BaseProjectile.h"

void UDestroyOnHitEffect::ApplyEffect_Implementation(const FAbilityEffectContext& Context)
{
    if (Context.Projectile)
    {
        Context.Projectile->Destroy();
    }
}
