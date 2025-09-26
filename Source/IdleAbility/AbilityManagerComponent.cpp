// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityManagerComponent.h"
#include "CustomCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "AbilityEffectData.h"
#include "Algo/RandomShuffle.h"

UAbilityManagerComponent::UAbilityManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAbilityManagerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAbilityManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Auto abilities
    for (int32 i = 0; i < EquippedAbilities.Num(); i++)
    {
        FAbilitySpec& Spec = EquippedAbilities[i];
        if (Spec.Ability && Spec.Ability->TriggerType == EAbilityTriggerType::Auto)
        {
            TryActivateAbility(i);
        }
    }
}

bool UAbilityManagerComponent::IsAbilityReady(const FAbilitySpec& Spec) const
{
    return (Spec.Ability != nullptr) && (GetWorld()->TimeSeconds >= Spec.CooldownEndTime);
}

void UAbilityManagerComponent::TryActivateAbility(int32 AbilityIndex)
{
    if (!EquippedAbilities.IsValidIndex(AbilityIndex)) return;

    FAbilitySpec& Spec = EquippedAbilities[AbilityIndex];
    if (!IsAbilityReady(Spec)) return;

    ACustomCharacter* Caster = Cast<ACustomCharacter>(GetOwner());
    if (!Caster) return;

    ExecuteAbility(Spec);

    // Cooldown ajusté par la stat
    float BaseCd = Spec.Ability->Cooldown;
    float Scalar = Spec.CooldownScalar; // Modif runtime par des ability comme frenzy
    float Cdr = 1.f - Caster->CooldownReduction;

    float FinalCooldown = BaseCd * Scalar * Cdr;
    if (FinalCooldown < 0.1f) FinalCooldown = 0.05f;

    Spec.CooldownEndTime = GetWorld()->TimeSeconds + FinalCooldown;

}

void UAbilityManagerComponent::ExecuteAbility(const FAbilitySpec& Spec)
{
    if (!Spec.Ability) return;

    ACustomCharacter* Caster = Cast<ACustomCharacter>(GetOwner());
    if (!Caster) return;

    // 1) Trouver les cibles depuis la data
    TArray<ACustomCharacter*> Targets;
    FindTargets(Spec.Ability, Caster, Targets);

    // Self-case: si Targeting=Self et aucune cible
    if (Spec.Ability->Targeting == EAbilityTargeting::Self && Targets.Num() == 0)
    {
        Targets.Add(Caster);
    }

    if (Targets.Num() == 0 && Spec.Ability->Targeting != EAbilityTargeting::Self)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s a lancé %s mais aucune cible trouvée."),
            *Caster->GetName(), *Spec.Ability->AbilityName.ToString());
        return;
    }

    for (UAbilityEffectData* EffectData : Spec.Ability->Effects)
    {
        if (!EffectData) continue;

        if (EffectData->TriggerPhase == EEffectTriggerPhase::OnCast)
        {
            UE_LOG(LogTemp, Warning, TEXT("[Ability] Trouvé effet %s (Phase=%s) dans %s"),
                *EffectData->GetClass()->GetName(),
                *UEnum::GetValueAsString(EffectData->TriggerPhase),
                *Spec.Ability->AbilityName.ToString());

            for (ACustomCharacter* Tgt : Targets)
            {
                FAbilityEffectContext Ctx;
                Ctx.Source = Caster;
                Ctx.Target = Tgt;
                Ctx.Ability = Spec.Ability;

                EffectData->ApplyEffect(Ctx);
            }
        }
    }


}

void UAbilityManagerComponent::FindTargets(const UAbilityData* Ability, ACustomCharacter* Caster, TArray<ACustomCharacter*>& OutTargets) const
{
    if (!Ability || !Caster) return;

    // Collecte brute des ennemis potentiels
    TArray<ACustomCharacter*> Candidates;
    for (TActorIterator<ACustomCharacter> It(GetWorld()); It; ++It)
    {
        ACustomCharacter* C = *It;
        if (!C || C == Caster) continue;

        // Filtre "ennemi" par TeamId
        if (C->TeamId == Caster->TeamId) continue;

        Candidates.Add(C);
    }

    if (Ability->Targeting == EAbilityTargeting::RandomEnemies)
    {
        Algo::RandomShuffle(Candidates);
        const int32 N = FMath::Min(Ability->TargetCount, Candidates.Num());
        for (int32 i = 0; i < N; ++i)
        {
            OutTargets.Add(Candidates[i]);
        }
    }
    else if (Ability->Targeting == EAbilityTargeting::SingleNearestEnemy)
    {
        float BestDistSq = TNumericLimits<float>::Max();
        ACustomCharacter* Best = nullptr;

        for (ACustomCharacter* C : Candidates)
        {
            float d = FVector::DistSquared(C->GetActorLocation(), Caster->GetActorLocation());
            if (d < BestDistSq && d <= Ability->Range * Ability->Range)
            {
                BestDistSq = d;
                Best = C;
            }
        }
        if (Best) OutTargets.Add(Best);
    }
    else if (Ability->Targeting == EAbilityTargeting::AllEnemiesInRange)
    {
        for (ACustomCharacter* C : Candidates)
        {
            float d = FVector::DistSquared(C->GetActorLocation(), Caster->GetActorLocation());
            if (d <= Ability->Range * Ability->Range)
            {
                OutTargets.Add(C);
            }
        }
    }
}

void UAbilityManagerComponent::GetEnemiesInRange(const ACustomCharacter* Origin, float Range, TArray<ACustomCharacter*>& Out) const
{
    Out.Reset();
    if (!Origin) return;

    const float RangeSq = (Range <= 0.f) ? FLT_MAX : Range * Range;

    for (TActorIterator<ACustomCharacter> It(GetWorld()); It; ++It)
    {
        ACustomCharacter* C = *It;
        if (!C || C == Origin) continue;
        if (!C->IsAlive()) continue;

        // Ennemis seulement (si tu as TeamId)
        if (C->TeamId == Origin->TeamId) continue;

        if (FVector::DistSquared(C->GetActorLocation(), Origin->GetActorLocation()) <= RangeSq)
        {
            Out.Add(C);
        }
    }
}
