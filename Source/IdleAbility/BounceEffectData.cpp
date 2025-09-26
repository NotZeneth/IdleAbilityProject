// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceEffectData.h"
#include "CustomCharacter.h"
#include "AbilityData.h"
#include "AbilityManagerComponent.h"
#include "BaseProjectile.h"
#include "Engine/World.h"
#include "EngineUtils.h"

void UBounceEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    ABaseProjectile* Projectile = Cast<ABaseProjectile>(Context.Projectile);
    if (!Projectile || !Context.Source || !Context.Target || !Context.Ability)
        return;

    // Chance effective = stat joueur × multiplicateur
    float EffectiveChance = Context.Source->BounceChance * ChanceMultiplier;
    float Roll = FMath::FRand();

    UE_LOG(LogTemp, Warning, TEXT("[Bounce] Roll=%.2f | Chance=%.2f"), Roll, EffectiveChance);

    if (Roll > EffectiveChance || Projectile->RemainingBounces <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Bounce raté ou plus de bounces restants -> projectile détruit"));
        Projectile->Destroy();
        return;
    }

    // Décrémente le compteur de rebonds
    Projectile->RemainingBounces--;

    // Récupère toutes les cibles potentielles
    TArray<ACustomCharacter*> Candidates;
    float Range = (Context.Ability->Range > 0) ? Context.Ability->Range : BounceRangeOverride;

    for (TActorIterator<ACustomCharacter> It(Context.Source->GetWorld()); It; ++It)
    {
        ACustomCharacter* C = *It;
        if (!C || C == Context.Source || !C->IsAlive()) continue;

        if (C == Context.Target) continue; // ignore la cible qu’on vient de toucher

        float DistSq = FVector::DistSquared(C->GetActorLocation(), Context.Target->GetActorLocation());
        if (DistSq <= Range * Range)
        {
            Candidates.Add(C);
        }
    }

    if (Candidates.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Aucun candidat trouvé -> projectile détruit"));
        Projectile->Destroy();
        return;
    }

    // Choisit une nouvelle cible aléatoire
    ACustomCharacter* NewTarget = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
    if (!NewTarget || !NewTarget->IsAlive())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Nouvelle cible invalide -> projectile détruit"));
        Projectile->Destroy();
        return;
    }

    // Redirige le projectile
    Projectile->Target = NewTarget;
    Projectile->InitialDirection =
        (NewTarget->GetActorLocation() - Projectile->GetActorLocation()).GetSafeNormal();

    UE_LOG(LogTemp, Warning, TEXT("[Bounce] Bounce réussi: %s -> %s (bounces restants=%d)"),
        *Context.Target->GetName(),
        *NewTarget->GetName(),
        Projectile->RemainingBounces);
}
