// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnProjectileEffectData.h"
#include "BaseProjectile.h"
#include "CustomCharacter.h"
#include "Engine/World.h"

bool USpawnProjectileEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (ProjectileOptions.Num() == 0 || !Context.Source)
        return false;

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

    FVector SpawnLoc = Context.Source->GetActorLocation();
    FRotator SpawnRot = Context.Source->GetActorRotation();

    ABaseProjectile* Proj = Context.Source->GetWorld()->SpawnActor<ABaseProjectile>(
        Chosen->ProjectileClass, SpawnLoc, SpawnRot, Params);

    if (!Proj)
        return false;

    // Application de l’offset défini dans le Blueprint du projectile
    Proj->SetActorLocation(SpawnLoc + Proj->SpawnOffset);

    Proj->Source = Context.Source;
    Proj->Target = Context.Target;
    Proj->Ability = Context.Ability;

    Proj->EffectsOnHit = Chosen->ExtraSubEffects;
    Proj->EffectsOnHit.Append(SubEffects);

    Proj->RemainingBounces = Context.Source->MaxBounces;

    return true;
}
