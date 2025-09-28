// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnProjectileEffectData.h"
#include "BaseProjectile.h"
#include "CustomCharacter.h"
#include "Engine/World.h"

bool USpawnProjectileEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (ProjectileOptions.Num() == 0 || !Context.Source)
        return false;

    // Choix du projectile selon les poids
    float TotalWeight = 0.f;
    for (const FWeightedProjectile& Opt : ProjectileOptions)
    {
        if (Opt.ProjectileClass)
            TotalWeight += FMath::Max(0.f, Opt.Weight);
    }
    if (TotalWeight <= 0.f)
        return false;

    float Roll = FMath::FRandRange(0.f, TotalWeight);
    const FWeightedProjectile* Chosen = nullptr;

    for (const FWeightedProjectile& Opt : ProjectileOptions)
    {
        if (!Opt.ProjectileClass) continue;
        Roll -= FMath::Max(0.f, Opt.Weight);
        if (Roll <= 0.f)
        {
            Chosen = &Opt;
            break;
        }
    }

    if (!Chosen) return false;

    FActorSpawnParameters Params;
    Params.Owner = Context.Source;
    Params.Instigator = Context.Source;

    FVector SpawnLoc = Context.Source->GetActorLocation() + FVector(0, 0, 50);
    FRotator SpawnRot = Context.Source->GetActorRotation();

    ABaseProjectile* Proj = Context.Source->GetWorld()->SpawnActor<ABaseProjectile>(
        Chosen->ProjectileClass, SpawnLoc, SpawnRot, Params);

    if (Proj)
    {
        Proj->Source = Context.Source;
        Proj->Target = Context.Target;
        Proj->Ability = Context.Ability;

        // On prend d’abord les extra, puis les subeffects "communs" déjà présents
        Proj->EffectsOnHit = Chosen->ExtraSubEffects;
        Proj->EffectsOnHit.Append(SubEffects);

        Proj->RemainingBounces = Context.Source->MaxBounces;

        UE_LOG(LogTemp, Warning, TEXT("[SpawnProjectile] %s a lancé %s"),
            *Context.Source->GetName(),
            *Chosen->ProjectileClass->GetName());
    }
    return true;
}
