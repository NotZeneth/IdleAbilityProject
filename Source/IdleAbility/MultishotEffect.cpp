// Fill out your copyright notice in the Description page of Project Settings.

// MultishotEffect.cpp

#include "MultishotEffect.h"
#include "AbilityData.h"
#include "CustomCharacter.h"
#include "EngineUtils.h"
#include "Algo/RandomShuffle.h"
#include "SpawnProjectileEffect.h" // important pour filtrer le bon type

void UMultishotEffect::ApplyEffect_Implementation(const FAbilityEffectContext& Context)
{
    if (!Context.Source || !Context.AbilityData) return;

    // 1) Chance de déclenchement
    if (FMath::FRand() > Context.Source->MultishotChance * Chance) return;

    // 2) Nombre max de tirs supplémentaires
    const int32 ExtraShots = FMath::Max(0, Context.Source->MultishotAmount);
    if (ExtraShots <= 0) return;

    // 3) Collecter toutes les cibles dans la portée
    TArray<ACustomCharacter*> Candidates;
    const float RangeSq = Context.AbilityData->Range * Context.AbilityData->Range;

    for (TActorIterator<ACustomCharacter> It(Context.Source->GetWorld()); It; ++It)
    {
        ACustomCharacter* C = *It;
        if (!C || C == Context.Source || !C->IsAlive()) continue;

        float DistSq = FVector::DistSquared(C->GetActorLocation(), Context.Source->GetActorLocation());
        if (DistSq <= RangeSq)
        {
            Candidates.Add(C);
        }
    }

    // 4) On évite de retaper la cible principale
    if (Context.Target) { Candidates.Remove(Context.Target); }

    if (bRandomizeTargets)
    {
        Algo::RandomShuffle(Candidates);
    }

    // 5) Limiter le nombre de tirs supplémentaires
    const int32 ShotsToSpawn = FMath::Min(ExtraShots, Candidates.Num());
    if (ShotsToSpawn <= 0) return;


    // 6) Spawn de projectiles sur les cibles choisies
    for (int32 i = 0; i < ShotsToSpawn; ++i)
    {
        ACustomCharacter* ExtraTarget = Candidates[i];
        if (!ExtraTarget || !ExtraTarget->IsAlive()) continue;

        UE_LOG(LogTemp, Warning, TEXT("Multishot: %s tire aussi sur %s"),
            *Context.Source->GetName(), *ExtraTarget->GetName());

        UE_LOG(LogTemp, Warning, TEXT("[Multishot] %s -> ExtraTarget=%s | ShotsToSpawn=%d"),
            Context.Source ? *Context.Source->GetName() : TEXT("null"),
            ExtraTarget ? *ExtraTarget->GetName() : TEXT("null"),
            ShotsToSpawn);


        // Ne rejoue QUE les effets qui spawnent un projectile
        for (UAbilityEffect* Effect : Context.AbilityData->Effects)
        {
            if (!Effect) continue;

            if (Effect->IsA(USpawnProjectileEffect::StaticClass()))
            {
                FAbilityEffectContext NewCtx(Context.Source, ExtraTarget, Context.AbilityData, nullptr);
                Effect->ApplyEffect(NewCtx);
            }
        }
    }
}

