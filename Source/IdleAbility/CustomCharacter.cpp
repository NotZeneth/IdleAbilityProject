// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCharacter.h"
#include "TimerManager.h"

ACustomCharacter::ACustomCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACustomCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHP = MaxHP;
}

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
        if (EffectiveReduction < 0) EffectiveReduction = 0;
        FinalDamage *= (1.f - EffectiveReduction);
    }
    else if (DamageType == EDamageType::Magical)
    {
        float EffectiveReduction = MagicalDmgReduction - IgnoreDmgReduction;
        if (EffectiveReduction < 0) EffectiveReduction = 0;
        FinalDamage *= (1.f - EffectiveReduction);
    }
    // Pure = pas de réduction

    CurrentHP = FMath::Clamp(CurrentHP - FinalDamage, 0.f, MaxHP);
    UE_LOG(LogTemp, Warning, TEXT("%s a maintenant %f HP"), *GetName(), CurrentHP);

    if (!IsAlive())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s est mort !"), *GetName());
        // IMPORTANT : on NE détruit PAS ici.
        // EnemyCharacter s’en charge (notification + Destroy()).
    }
}

bool ACustomCharacter::IsAlive()
{
    return CurrentHP > 0.f;
}

void ACustomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}