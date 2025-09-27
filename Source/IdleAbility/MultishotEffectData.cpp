// Fill out your copyright notice in the Description page of Project Settings.

#include "MultishotEffectData.h"
#include "CustomCharacter.h"
#include "AbilityData.h"
#include "AbilityManagerComponent.h"
#include "SpawnProjectileEffectData.h"
#include "Algo/RandomShuffle.h"

void UMultishotEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    ACustomCharacter* Caster = Context.Source;
    if (!Caster || !Context.Ability) return;

    UAbilityManagerComponent* Manager = Caster->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager) return;

    // Roll chance
    float TotalChance = Caster->MultishotChance * ChanceMultiplier;
    if (FMath::FRand() > TotalChance)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Multishot] Raté (chance=%.2f)"), TotalChance);
        return;
    }

    // Collecter les ennemis en range
    TArray<ACustomCharacter*> Candidates;
    Manager->GetEnemiesInRange(Caster, Context.Ability->Range, Candidates);

    // Retirer la cible principale si elle existe
    if (Context.Target)
    {
        Candidates.Remove(Context.Target);
    }

    if (Candidates.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Multishot] Pas d'autres cibles en range"));
        return;
    }

    // Mélanger aléatoirement
    Algo::RandomShuffle(Candidates);

    // Déterminer combien de tirs
    int32 ShotsToSpawn = FMath::Min(Caster->MultishotAmount, Candidates.Num());

    UE_LOG(LogTemp, Warning, TEXT("[Multishot] Proc -> %d projectiles supplémentaires"), ShotsToSpawn);

    // Spawner les projectiles supplémentaires
    for (int32 i = 0; i < ShotsToSpawn; i++)
    {
        ACustomCharacter* ExtraTarget = Candidates[i];
        if (!ExtraTarget) continue;

        FAbilityEffectContext NewCtx;
        NewCtx.Source = Caster;
        NewCtx.Target = ExtraTarget;
        NewCtx.Ability = Context.Ability;

        // Rejouer uniquement les effets de type SpawnProjectile
        for (UAbilityEffectData* Effect : Context.Ability->Effects)
        {
            if (Effect && Effect->IsA<USpawnProjectileEffectData>())
            {
                Effect->ApplyEffect(NewCtx);
            }
        }
    }
}
