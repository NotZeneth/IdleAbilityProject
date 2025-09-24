// Fill out your copyright notice in the Description page of Project Settings.


#include "MultishotEffect.h"
#include "AbilityData.h"
#include "AbilityEffect.h"
#include "CustomCharacter.h"
#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"

// forward si besoin
class USpawnProjectileEffect;

void UMultishotEffect::ApplyEffect_Implementation(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData)
{
    if (!Source || !AbilityData) return;

    // 1) Roll de chance
    if (FMath::FRand() > Chance) return;

    // 2) Nombre de tirs additionnels autorisés par le joueur
    const int32 ExtraShots = FMath::Max(0, Source->MultishotAmount);
    if (ExtraShots <= 0) return;

    // 3) Collecter TOUTES les cibles ennemies dans le Range de l’ability
    TArray<ACustomCharacter*> Candidates;
    const float RangeSq = AbilityData->Range * AbilityData->Range;

    for (TActorIterator<ACustomCharacter> It(Source->GetWorld()); It; ++It)
    {
        ACustomCharacter* C = *It;
        if (!C || C == Source || !C->IsAlive()) continue;

        // Si tu as un TeamId, filtre ici (ex: if (C->TeamId == Source->TeamId) continue;)
        const float DistSq = FVector::DistSquared(C->GetActorLocation(), Source->GetActorLocation());
        if (DistSq <= RangeSq)
        {
            Candidates.Add(C);
        }
    }

    // 4) Ne pas re-tirer sur la cible principale
    if (Target) { Candidates.Remove(Target); }

    if (bRandomizeTargets)
    {
        Algo::RandomShuffle(Candidates);
    }

    // 5) Limiter au nombre demandé
    const int32 ShotsToSpawn = FMath::Min(ExtraShots, Candidates.Num());
    if (ShotsToSpawn <= 0) return;

    // 6) Pour chaque cible supplémentaire, REJOUER UNIQUEMENT les effets OnCast de l’ability
    //    et ignorer les Multishot/effets OnCast qui ne doivent pas se re-déclencher.
    for (int32 i = 0; i < ShotsToSpawn; ++i)
    {
        ACustomCharacter* ExtraTarget = Candidates[i];
        if (!ExtraTarget || !ExtraTarget->IsAlive()) continue;

        UE_LOG(LogTemp, Warning, TEXT("Multishot: %s tire aussi sur %s"),
            *Source->GetName(), *ExtraTarget->GetName());

        for (UAbilityEffect* Effect : AbilityData->Effects)
        {
            if (!Effect || Effect->TriggerPhase != EEffectTriggerPhase::OnCast)
                continue;

            // Éviter la récursion: ne rejoue pas Multishot lui-même
            if (Effect->IsA(UMultishotEffect::StaticClass()))
                continue;

            Effect->ApplyEffect(Source, ExtraTarget, AbilityData);
        }
    }
}
