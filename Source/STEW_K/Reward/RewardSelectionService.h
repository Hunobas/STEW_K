// RewardSelectionService.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RewardSelectionService.generated.h"

class UWeaponReward;
class UPassiveItemReward;
class UPlayerPowerUpReward;
class APlanetPawn;

UCLASS(Blueprintable)
class STEW_K_API URewardSelectionService : public UObject
{
    GENERATED_BODY()
public:
    URewardSelectionService();

    void InitializeRewards();
    
    UFUNCTION(BlueprintCallable)
    TArray<UObject*> GetRandomRewards();

    UFUNCTION(BlueprintCallable)
    void AcquireReward(UObject* Reward);

private:
    UPROPERTY(EditDefaultsOnly, Category = "보상")
    int32 MaxRewards = 3;

    UPROPERTY(EditDefaultsOnly, Category = "보상")
    TArray<TSubclassOf<UWeaponReward>> WeaponClasses;
    UPROPERTY()
    TMap<UWeaponReward*, int32> AvailableWeaponRewards;
    UPROPERTY()
    TArray<UWeaponReward*> TempWeaponRewards;

    UPROPERTY(EditDefaultsOnly, Category = "보상")
    TArray<TSubclassOf<UPassiveItemReward>> PassiveItemClasses;
    UPROPERTY()
    TMap<UPassiveItemReward*, int32> AvailablePassiveItemRewards;
    UPROPERTY()
    TArray<UPassiveItemReward*> TempPassiveItemRewards;

    UPROPERTY(EditDefaultsOnly, Category = "보상")
    TArray<TSubclassOf<UPlayerPowerUpReward>> PlayerPowerUpClasses;
    UPROPERTY()
    TArray<UPlayerPowerUpReward*> AvailablePlayerPowerUpRewards;
    UPROPERTY()
    TArray<UPlayerPowerUpReward*> TempPlayerPowerUpRewards;

    void InitializeWeaponRewards();
    void InitializePassiveItemRewards();
    void InitializePlayerPowerUpRewards();

    UObject* GetRandomWeaponRewardOrNull();
    UObject* GetRandomPassiveItemRewardOrNull();
    UObject* GetRandomPlayerPowerUpRewardOrNull();
    
    void UpdateItemPoolWeight();

    void HandleWeaponReward(UWeaponReward* WeaponReward);
    void HandlePassiveItemReward(UPassiveItemReward* PassiveItemReward);
    void HandlePlayerPowerUpReward(UPlayerPowerUpReward* PlayerPowerUpReward);

    APlanetPawn* PlayerPawn;

    int32 RemainWeaponSlots = 4;
    int32 RemainPassiveItemSlots = 6;
    int32 ItemPoolWeight = 0;
    float PWeaponAppear = 0.6f;
    float PPassiveItemAppear = 0.95f;
};