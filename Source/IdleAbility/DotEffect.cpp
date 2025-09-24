// Fill out your copyright notice in the Description page of Project Settings.


#include "DotEffect.h"
#include "AbilityData.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UDotEffect::ApplyEffect_Implementation(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData)
{
    if (!Target || !AbilityData) return;

    UWorld* World = Target->GetWorld();
    if (!World) return;

    int32 NumTicks = FMath::FloorToInt(Duration / TickInterval);

    for (int32 i = 0; i < NumTicks; i++)
    {
        FTimerHandle TimerHandle;
        FTimerDelegate Delegate;

        Delegate.BindUObject(this, &UDotEffect::ApplyTick, Source, Target, AbilityData);

        World->GetTimerManager().SetTimer(
            TimerHandle,
            Delegate,
            TickInterval * (i + 1),
            false
        );
    }

    UE_LOG(LogTemp, Warning, TEXT("%s applique un DoT (%s) sur %s : %.1f dmg/tick, %d ticks"),
        *Source->GetName(),
        *UEnum::GetValueAsString(AbilityData->AbilityType),
        *Target->GetName(),
        AbilityData->PrimaryValue,
        NumTicks
    );
}

void UDotEffect::ApplyTick(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData)
{
    if (!Target || !Target->IsAlive()) return;

    Target->TakeCustomDamage(AbilityData->PrimaryValue, AbilityData->AbilityType, Source);

    UE_LOG(LogTemp, Warning, TEXT("DoT tick : %s prend %.1f dégâts (%s)"),
        *Target->GetName(),
        AbilityData->PrimaryValue,
        *UEnum::GetValueAsString(AbilityData->AbilityType));
}