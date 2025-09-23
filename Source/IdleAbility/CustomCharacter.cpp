// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacter.h"
///
#include "TimerManager.h"


// Sets default values
ACustomCharacter::ACustomCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACustomCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHP = MaxHP;
}

// Called every frame
void ACustomCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsAlive() && HpRegenPercent > 0.f)
    {
        RegenHealthOverTime(DeltaTime);
    }
}

void ACustomCharacter::RegenHealthOverTime(float DeltaTime)
{
    Heal(MaxHP * HpRegenPercent * DeltaTime);
}

void ACustomCharacter::Heal(float HealAmount)
{
    if (!IsAlive()) return;

    CurrentHP = FMath::Clamp(CurrentHP + HealAmount, 0.f, MaxHP);
}

void ACustomCharacter::TakeCustomDamage(float DamageAmount, EDamageType DamageType, AActor* Source)
{
    if (!IsAlive()) return;

    float FinalDamage = DamageAmount;

    if (DamageType == EDamageType::Physical)
    {
        float EffectiveReduction = PhysicalDmgReduction - IgnoreDmgReduction;
        if (EffectiveReduction < 0) EffectiveReduction = 0; // A retirer si on veut malus une defense negative.
        FinalDamage *= (1.f - EffectiveReduction);
    }
    else if (DamageType == EDamageType::Magical)
    {
        float EffectiveReduction = MagicalDmgReduction - IgnoreDmgReduction;
        if (EffectiveReduction < 0) EffectiveReduction = 0; // A retirer si on veut malus une defense negative.
        FinalDamage *= (1.f - EffectiveReduction);
    }
    // Pure = pas de réduction

    CurrentHP = FMath::Clamp(CurrentHP - FinalDamage, 0.f, MaxHP);
    UE_LOG(LogTemp, Warning, TEXT("New health is %f"), CurrentHP);

    if (!IsAlive())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s est mort !"), *GetName());
    }
}

bool ACustomCharacter::IsAlive()
{
    return CurrentHP > 0.f;
}

// Called to bind functionality to input
void ACustomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}