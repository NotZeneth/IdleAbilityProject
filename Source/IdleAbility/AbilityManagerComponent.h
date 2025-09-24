// AbilityManagerComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityData.h"
#include "AbilityManagerComponent.generated.h"

USTRUCT(BlueprintType)
struct FAbilitySpec
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAbilityData* Ability = nullptr;

    // Temps où le cooldown se termine
    float CooldownEndTime = 0.f;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class IDLEABILITY_API UAbilityManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAbilityManagerComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Liste d'abilities équipées
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TArray<FAbilitySpec> EquippedAbilities;

    // Activer une ability par index
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void TryActivateAbility(int32 AbilityIndex);

    // Vérifie cooldown
    bool IsAbilityReady(const FAbilitySpec& Spec) const;

public:
    void ExecuteAbility(const FAbilitySpec& Spec);

    // Renvoie les cibles en fonction du Targeting du DataAsset
    void FindTargets(const UAbilityData* Ability, class ACustomCharacter* Caster, TArray<ACustomCharacter*>& OutTargets) const;

    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void GetEnemiesInRange(const class ACustomCharacter* Origin, float Range, TArray<class ACustomCharacter*>& Out) const;

};
