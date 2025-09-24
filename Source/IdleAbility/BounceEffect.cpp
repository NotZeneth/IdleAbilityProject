// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceEffect.h"
#include "CustomCharacter.h"
#include "BaseProjectile.h"
#include "AbilityManagerComponent.h"

void UBounceEffect::ApplyEffect_Implementation(const FAbilityEffectContext& Context)
{
    if (!Context.Source || !Context.Target || !Context.AbilityData || !Context.Projectile)
        return;

    // 1) Jet de chance
    const float Roll = FMath::FRand();
    const float EffectiveChance = Context.Source->BounceChance * ChanceMultiplier;

    UE_LOG(LogTemp, Warning, TEXT("[Bounce] Roll=%.2f | Chance=%.2f"),
        Roll, EffectiveChance);

    if (Roll > EffectiveChance)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Bounce échoué -> projectile détruit"));
        Context.Projectile->Destroy();
        return;
    }

    // 2) Récup manager
    UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();
    if (!Manager)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Pas de AbilityManager sur %s"),
            *Context.Source->GetName());
        Context.Projectile->Destroy();
        return;
    }

    // 3) Chercher d’autres ennemis dans le range, depuis la source (le joueur)
    TArray<ACustomCharacter*> Candidates;
    Manager->GetEnemiesInRange(Context.Source, Context.AbilityData->Range, Candidates);

    // Exclure la cible courante
    Candidates.Remove(Context.Target);


    UE_LOG(LogTemp, Warning, TEXT("[Bounce] %d candidats trouvés après exclusion de %s"),
        Candidates.Num(),
        *Context.Target->GetName());

    if (Candidates.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Aucun candidat -> projectile détruit"));
        Context.Projectile->Destroy();
        return;
    }

    // 4) Pick aléatoire
    ACustomCharacter* NewTarget = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
    if (!NewTarget || !NewTarget->IsAlive())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Nouvelle cible invalide -> projectile détruit"));
        Context.Projectile->Destroy();
        return;
    }

    // 5) Log + redirection
    UE_LOG(LogTemp, Warning, TEXT("[Bounce] Bounce réussi: %s -> %s"),
        *Context.Target->GetName(),
        *NewTarget->GetName());

    Context.Projectile->Target = NewTarget;
    Context.Projectile->InitialDirection =
        (NewTarget->GetActorLocation() - Context.Projectile->GetActorLocation()).GetSafeNormal();
}
