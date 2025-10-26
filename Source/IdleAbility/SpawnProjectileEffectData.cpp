// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnProjectileEffectData.h"
#include "BaseProjectile.h"
#include "CustomCharacter.h"
#include "AbilityData.h"
#include "AbilityManagerComponent.h"
#include "Engine/World.h"
#include "Algo/RandomShuffle.h"

bool USpawnProjectileEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    if (ProjectileOptions.Num() == 0 || !Context.Source)
        return false;

    ACustomCharacter* Caster = Context.Source;
    UAbilityManagerComponent* Manager = Caster->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager)
        return false;

    // --- Sélection du projectile ---
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

    // --- Projectile principal (la vraie cible) ---
    auto SpawnSingleProjectile = [&](ACustomCharacter* Target, float YawOffset)
        {
            if (!Target) return;

            FActorSpawnParameters Params;
            Params.Owner = Caster;
            Params.Instigator = Caster;

            FVector SpawnLoc = Caster->GetActorLocation();
            FRotator SpawnRot = (Target->GetActorLocation() - SpawnLoc).Rotation();
            SpawnRot.Yaw += YawOffset;

            ABaseProjectile* Proj = Caster->GetWorld()->SpawnActor<ABaseProjectile>(
                Chosen->ProjectileClass, SpawnLoc, SpawnRot, Params);

            if (!Proj)
                return;

            Proj->SetActorLocation(SpawnLoc + Proj->SpawnOffset);
            Proj->Source = Caster;
            Proj->Target = Target;
            Proj->Ability = Context.Ability;
            Proj->EffectsOnHit = Chosen->ExtraSubEffects;
            Proj->EffectsOnHit.Append(SubEffects);
            Proj->RemainingBounces = Caster->MaxBounces;

            Proj->InitializeProjectile();

            UE_LOG(LogTemp, Log, TEXT("[SpawnProjectile] Tir vers %s (YawOffset %.1f°)"),
                *Target->GetName(), YawOffset);
        };

    // --- Tir principal ---
    SpawnSingleProjectile(Context.Target, 0.f);

    // --- Gestion du Multishot RNG ---
    if (bEnableMultishot)
    {
        const float RollChance = Caster->MultishotChance * ChanceMultiplier;
        const float RandRoll = FMath::FRand();

        if (RandRoll <= RollChance)
        {
            // Cherche d'autres ennemis
            TArray<ACustomCharacter*> Candidates;
            Manager->GetEnemiesInRange(Caster, Context.Ability->Range, Candidates);

            // Retirer la cible principale de la liste
            if (Context.Target)
                Candidates.Remove(Context.Target);

            if (Candidates.Num() == 0)
            {
                UE_LOG(LogTemp, Warning, TEXT("[Multishot] Aucune autre cible trouvee"));
                return true;
            }

            Algo::RandomShuffle(Candidates);

            // MultishotAmount = nombre de projectiles supplémentaires
            const int32 ExtraShots = FMath::Min(Caster->MultishotAmount, Candidates.Num());

            UE_LOG(LogTemp, Warning, TEXT("[Multishot] Proc -> %d projectiles supplementaires"), ExtraShots);

            // Tirer sur d'autres cibles UNIQUES
            for (int32 i = 0; i < ExtraShots; ++i)
            {
                ACustomCharacter* ExtraTarget = Candidates[i];
                if (!ExtraTarget || ExtraTarget == Context.Target)
                    continue;

                float YawOffset = FMath::RandRange(-SpreadAngleDeg, SpreadAngleDeg);
                SpawnSingleProjectile(ExtraTarget, YawOffset);
            }
        }

        else
        {
            UE_LOG(LogTemp, Verbose, TEXT("[Multishot] Échec (%.2f > %.2f)"), RandRoll, RollChance);
        }
    }

    return true;
}
