// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnProjectileEffectData.h"
#include "BaseProjectile.h"
#include "CustomCharacter.h"
#include "Engine/World.h"

bool USpawnProjectileEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (!ProjectileClass || !Context.Source) return true;

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
        Proj->Ability = Context.Ability;
        Proj->EffectsOnHit = SubEffects;

        Proj->RemainingBounces = Context.Source->MaxBounces; // Bon ca, autant le donner car p'tet un projectile devient bounce + tard, better safe

        UE_LOG(LogTemp, Warning, TEXT("[SpawnProjectile] %s a lancé un projectile %s"),
            *Context.Source->GetName(),
            *ProjectileClass->GetName());
    }

    return true;
}
