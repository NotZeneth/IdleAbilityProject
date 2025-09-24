// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnProjectileEffect.h"
#include "AbilityData.h"
#include "BaseProjectile.h"
#include "CustomCharacter.h"

void USpawnProjectileEffect::ApplyEffect_Implementation(const FAbilityEffectContext& Context)
{
    if (!ProjectileClass || !Context.Source) return;

    FActorSpawnParameters Params;
    Params.Owner = Context.Source;
    Params.Instigator = Context.Source;

    FVector SpawnLoc = Context.Source->GetActorLocation() + FVector(0, 0, 50);
    FRotator SpawnRot = Context.Source->GetActorRotation();

    ABaseProjectile* Proj = Context.Source->GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params);
    if (Proj)
    {
        Proj->Source = Context.Source;
        Proj->Target = Context.Target;
        Proj->AbilityData = Context.AbilityData;
        Proj->EffectsOnHit = SubEffects;

        UE_LOG(LogTemp, Warning, TEXT("%s a lancé un projectile (%s)"),
            *Context.Source->GetName(),
            *ProjectileClass->GetName());
    }
}
