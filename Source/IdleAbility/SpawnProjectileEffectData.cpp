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
        // 1) Detecter si des upgrades existent sur cette ability
        bool bHasChanceUpgrade = false;
        bool bHasAmountUpgrade = false;

        if (Context.Ability)
        {
            const FAbilityUpgradeSet& Up = Context.Ability->BaseUpgrades;
            bHasChanceUpgrade = (Up.MultishotChance.EffectValues.Num() > 0);
            bHasAmountUpgrade = (Up.MultishotAmount.EffectValues.Num() > 0);
        }

        // 2) Lire les valeurs
        float Chance = MultishotChance;                 // fallback: chance definie sur l'effet (0..1)
        int32 Amount = MultishotAmount;                  // fallback: amount defini sur l'effet

        if (Manager && Context.Ability)
        {
            if (bHasChanceUpgrade)
            {
                const float UpChance = Manager->GetUpgradeValue(Context.Ability, TEXT("MultishotChance"));
                // on suppose que les tracks d’upgrade stockent des chances 0..1
                Chance = FMath::Clamp(UpChance, 0.f, 1.f);
            }

            if (bHasAmountUpgrade)
            {
                const float UpAmount = Manager->GetUpgradeValue(Context.Ability, TEXT("MultishotAmount"));
                Amount = FMath::Max(0, static_cast<int32>(FMath::RoundToInt(UpAmount)));
            }
        }

        // 3) RNG sur la chance calculee
        const float RandRoll = FMath::FRand();
        if (RandRoll <= Chance && Amount > 0)
        {
            // Cherche d'autres ennemis
            TArray<ACustomCharacter*> Candidates;
            Manager->GetEnemiesInRange(Caster, Context.Ability->Range, Candidates);

            // Retirer la cible principale
            if (Context.Target)
                Candidates.Remove(Context.Target);

            if (Candidates.Num() == 0)
            {
                UE_LOG(LogTemp, Warning, TEXT("[Multishot] Aucune autre cible trouvee"));
                return true;
            }

            Algo::RandomShuffle(Candidates);

            // Limiter par le nombre de candidats
            const int32 ExtraShots = FMath::Min(Amount, Candidates.Num());
            UE_LOG(LogTemp, Warning, TEXT("[Multishot] Proc -> %d projectiles supplementaires (Chance=%.2f)"), ExtraShots, Chance);

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
            UE_LOG(LogTemp, Verbose, TEXT("[Multishot] Echec (roll=%.2f > chance=%.2f) ou amount=0"), RandRoll, Chance);
        }
    }
    return true;
}
