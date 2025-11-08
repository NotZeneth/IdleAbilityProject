// Fill out your copyright notice in the Description page of Project Settings.

#include "BounceEffectData.h"
#include "CustomCharacter.h"
#include "AbilityData.h"
#include "AbilityManagerComponent.h"
#include "BaseProjectile.h"
#include "Engine/World.h"
#include "EngineUtils.h"

bool UBounceEffectData::ApplyEffect(const FAbilityEffectContext& Context) const
{
    ABaseProjectile* Projectile = Cast<ABaseProjectile>(Context.Projectile);
    if (!Projectile || !Context.Source || !Context.Target || !Context.Ability)
        return true;

    UAbilityManagerComponent* Manager = Context.Source->FindComponentByClass<UAbilityManagerComponent>();

    float Chance = BounceChance;
    int32 Count = BounceCount;

    if (Manager && Context.Ability)
    {
        const float UpChance = Manager->GetUpgradeValue(Context.Ability, TEXT("BounceChance"));
        const float UpAmount = Manager->GetUpgradeValue(Context.Ability, TEXT("BounceAmount"));

        // Remplace si le data asset a une upgrade de bounce
        const FAbilityUpgradeSet& Up = Context.Ability->BaseUpgrades;
        if (Up.BounceChance.EffectValues.Num() > 0)
            Chance = FMath::Clamp(UpChance, 0.f, 1.f);
        if (Up.BounceAmount.EffectValues.Num() > 0)
            Count = FMath::Max(0, static_cast<int32>(FMath::RoundToInt(UpAmount)));
    }

    const float Roll = FMath::FRand();
    UE_LOG(LogTemp, Warning, TEXT("[Bounce] Roll=%.2f | Chance=%.2f"), Roll, Chance);

    if (Roll > Chance)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Bounce rate -> projectile détruit (chance %.2f)"), Chance);
        Projectile->Destroy();
        return true;
    }

    if (Projectile->RemainingBounces <= 0 || Count <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Aucun rebond restant -> projectile détruit"));
        Projectile->Destroy();
        return true;
    }

    Projectile->RemainingBounces = FMath::Min(Projectile->RemainingBounces, Count) - 1;

    TArray<ACustomCharacter*> Candidates;
    const float Range = (Context.Ability->Range > 0) ? Context.Ability->Range : BounceRangeOverride;

    for (TActorIterator<ACustomCharacter> It(Context.Source->GetWorld()); It; ++It)
    {
        ACustomCharacter* C = *It;
        if (!C || C == Context.Source || !C->IsAlive()) continue;
        if (C == Context.Target) continue;

        const float DistSq = FVector::DistSquared(C->GetActorLocation(), Context.Target->GetActorLocation());
        if (DistSq <= Range * Range)
        {
            Candidates.Add(C);
        }
    }

    if (Candidates.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Aucun candidat trouvé -> projectile détruit"));
        Projectile->Destroy();
        return true;
    }

    ACustomCharacter* NewTarget = Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
    if (!NewTarget || !NewTarget->IsAlive())
    {
        UE_LOG(LogTemp, Warning, TEXT("[Bounce] Nouvelle cible invalide -> projectile détruit"));
        Projectile->Destroy();
        return true;
    }

    Projectile->Target = NewTarget;

    if (Projectile->MovementType == EProjectileMovementType::Homing)
        Projectile->InitialDirection = FVector(0.f, 0.f, 1.f).GetSafeNormal();
    else
        Projectile->InitialDirection = FVector(0.f, 0.f, 1.f);

    UE_LOG(LogTemp, Warning, TEXT("[Bounce] Bounce réussi: %s -> %s (bounces restants=%d)"),
        *Context.Target->GetName(),
        *NewTarget->GetName(),
        Projectile->RemainingBounces);

    return true;
}
