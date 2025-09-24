// Fill out your copyright notice in the Description page of Project Settings.

#include "DotEffect.h"
#include "CustomCharacter.h"
#include "AbilityData.h"
#include "TimerManager.h"

void UDotEffect::ApplyEffect_Implementation(const FAbilityEffectContext& Context)
{
    if (!Context.Target || !Context.Target->IsAlive() || !Context.Source)
        return;

    if (UWorld* World = Context.Source->GetWorld())
    {
        FAbilityEffectContext LocalContext = Context; // copie par valeur

        // Timer pour ticks
        FTimerHandle TickHandle;
        World->GetTimerManager().SetTimer(
            TickHandle,
            FTimerDelegate::CreateUObject(this, &UDotEffect::ApplyTick, LocalContext),
            TickInterval,
            true
        );

        // Timer pour stop après la durée
        FTimerHandle LifetimeHandle;
        World->GetTimerManager().SetTimer(
            LifetimeHandle,
            FTimerDelegate::CreateLambda([World, TickHandle]() mutable
                {
                    World->GetTimerManager().ClearTimer(TickHandle);
                }),
            Duration,
            false
        );
    }
}

void UDotEffect::ApplyTick(const FAbilityEffectContext Context)
{
    if (!Context.Target || !Context.Target->IsAlive())
        return;

    float dmg = Context.AbilityData ? Context.AbilityData->PrimaryValue : 0.f;
    Context.Target->TakeCustomDamage(dmg, Context.AbilityData->AbilityType, Context.Source);

    UE_LOG(LogTemp, Warning, TEXT("[DOT] Tick on %s | Dmg=%.1f | Source=%s"),
        *Context.Target->GetName(), dmg, *Context.Source->GetName());
}
