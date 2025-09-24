// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnProjectileEffect.h"
#include "AbilityData.h"
#include "BaseProjectile.h"
#include "CustomCharacter.h"

void USpawnProjectileEffect::ApplyEffect_Implementation(ACustomCharacter* Source, ACustomCharacter* Target, const UAbilityData* AbilityData)
{
    if (!ProjectileClass || !Source) return;

    FActorSpawnParameters Params;
    Params.Owner = Source;
    Params.Instigator = Source;

    FVector SpawnLoc = Source->GetActorLocation() + FVector(0, 0, 50); // léger offset
    FRotator SpawnRot = Source->GetActorRotation();

    ABaseProjectile* Proj = Source->GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params);
    if (Proj)
    {
        Proj->Source = Source;
        Proj->Target = Target;
        Proj->AbilityData = AbilityData;
        Proj->EffectsOnHit = SubEffects;

        UE_LOG(LogTemp, Warning, TEXT("%s a lancé un projectile (%s)"),
            *Source->GetName(),
            *ProjectileClass->GetName());
    }
}
