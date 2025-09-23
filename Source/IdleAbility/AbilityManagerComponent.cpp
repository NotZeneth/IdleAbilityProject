// AbilityManagerComponent.cpp

#include "AbilityManagerComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "CustomCharacter.h"

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

    ExecuteAbility(Spec);

    // Mettre en cooldown
    Spec.CooldownEndTime = GetWorld()->TimeSeconds + Spec.Ability->Cooldown;
}

void UAbilityManagerComponent::ExecuteAbility(const FAbilitySpec& Spec)
{
    if (!Spec.Ability) return;

    AActor* Owner = GetOwner();
    ACustomCharacter* Caster = Cast<ACustomCharacter>(Owner);

    if (!Caster) return;

    // Exemple très simple : ability = dégâts directs sur la première cible trouvée
    if (Spec.Ability->AbilityName.EqualTo(FText::FromString("Basic Attack")))
    {
        // TODO : chercher une vraie cible. Ici on fait juste log.
        UE_LOG(LogTemp, Warning, TEXT("%s lance Basic Attack (%.1f dégâts)"),
            *Caster->GetName(), Spec.Ability->PrimaryValue);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%s utilise %s"),
            *Caster->GetName(), *Spec.Ability->AbilityName.ToString());
    }
}
