// RewardSelectionService.cpp
#include "Kismet/GameplayStatics.h"

#include "WeaponReward.h"
#include "PassiveItemReward.h"
#include "PlayerPowerUpReward.h"
#include "../Player/PlanetPawn.h"
#include "../Weapon/WeaponPawn.h"

#include "RewardSelectionService.h"

URewardSelectionService::URewardSelectionService()
{
    InitializeRewards();
}

void URewardSelectionService::InitializeRewards()
{
    InitializeWeaponRewards();
    InitializePassiveItemRewards();
    InitializePlayerPowerUpRewards();
    UpdateItemPoolWeight();
    PlayerPawn = Cast<APlanetPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
}

TArray<UObject*> URewardSelectionService::GetRandomRewards()
{
    TArray<UObject*> SelectedRewards;

    TempWeaponRewards.Empty();
    TempPassiveItemRewards.Empty();
    TempPlayerPowerUpRewards.Empty();

    if (RemainWeaponSlots != 0)
    {
        AvailableWeaponRewards.GetKeys(TempWeaponRewards);
    }
    if (RemainPassiveItemSlots != 0)
    {
        AvailablePassiveItemRewards.GetKeys(TempPassiveItemRewards);
    }
    TempPlayerPowerUpRewards = AvailablePlayerPowerUpRewards;

    while (SelectedRewards.Num() < MaxRewards)
    {
        float RandomValue = FMath::FRand();
        UObject* Reward = nullptr;

        if (RandomValue < PWeaponAppear && TempWeaponRewards.Num() > 0)
        {
            Reward = GetRandomWeaponRewardOrNull();
            TempWeaponRewards.Remove(Cast<UWeaponReward>(Reward));
        }
        else if (RandomValue < PPassiveItemAppear && TempPassiveItemRewards.Num() > 0)
        {
            Reward = GetRandomPassiveItemRewardOrNull();
            TempPassiveItemRewards.Remove(Cast<UPassiveItemReward>(Reward));
        }
        else if (TempPlayerPowerUpRewards.Num() > 0)
        {
            Reward = GetRandomPlayerPowerUpRewardOrNull();
            TempPlayerPowerUpRewards.Remove(Cast<UPlayerPowerUpReward>(Reward));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Reward selected nothing."));
            break;
        }

        if (Reward)
        {
            SelectedRewards.AddUnique(Reward);
        }
    }
    return SelectedRewards;
}

UObject* URewardSelectionService::GetRandomWeaponRewardOrNull()
{
    if (RemainWeaponSlots == 0)
    {
        return nullptr;
    }

    int32 RandomIndex = FMath::RandRange(0, TempWeaponRewards.Num() - 1);
    UWeaponReward* SelectedWeapon = TempWeaponRewards[RandomIndex];

    SelectedWeapon->CurrentLevel = AvailableWeaponRewards[SelectedWeapon] + 1;
    SelectedWeapon->SetDescriptionByLevel(SelectedWeapon->CurrentLevel);
    return SelectedWeapon;
}

UObject* URewardSelectionService::GetRandomPassiveItemRewardOrNull()
{
    if (RemainPassiveItemSlots == 0)
    {
        return nullptr;
    }

    int32 RandomValue = FMath::RandRange(1, ItemPoolWeight);
    int32 AccumulatedWeight = 0;

    for (UPassiveItemReward* Item : TempPassiveItemRewards)
    {
        AccumulatedWeight += Item->Rarity;
        if (RandomValue <= AccumulatedWeight)
        {
            Item->CurrentLevel = AvailablePassiveItemRewards[Item] + 1;
            return Item;
        }
    }

    return nullptr;
}

UObject* URewardSelectionService::GetRandomPlayerPowerUpRewardOrNull()
{
    int32 RandomIndex = FMath::RandRange(0, TempPlayerPowerUpRewards.Num() - 1);
    return TempPlayerPowerUpRewards[RandomIndex];
}

void URewardSelectionService::AcquireReward(UObject* Reward)
{
    if (UWeaponReward* WeaponReward = Cast<UWeaponReward>(Reward))
    {
        if (AvailableWeaponRewards[WeaponReward] == 0)
        {
            RemainWeaponSlots--;
        }
        AvailableWeaponRewards[WeaponReward] += 1;
        if (WeaponReward->IsMaxLevel())
        {
            AvailableWeaponRewards.Remove(WeaponReward);
        }
        HandleWeaponReward(WeaponReward);
    }
    else if (UPassiveItemReward* PassiveItemReward = Cast<UPassiveItemReward>(Reward))
    {
        if (AvailablePassiveItemRewards[PassiveItemReward] == 0)
        {
            RemainPassiveItemSlots--;
        }
        AvailablePassiveItemRewards[PassiveItemReward] += 1;
        if (PassiveItemReward->IsMaxLevel())
        {
            AvailablePassiveItemRewards.Remove(PassiveItemReward);
        }
        UpdateItemPoolWeight();
        HandlePassiveItemReward(PassiveItemReward);
    }
    else if (UPlayerPowerUpReward* PlayerPowerUpReward = Cast<UPlayerPowerUpReward>(Reward))
    {
        HandlePlayerPowerUpReward(PlayerPowerUpReward);
    }
}

void URewardSelectionService::InitializeWeaponRewards()
{
    AvailableWeaponRewards.Empty();
    for (int32 i = 0; i < WeaponClasses.Num(); ++i)
    {
        if (WeaponClasses[i])
        {
            FString WeaponName = FString::Printf(TEXT("WeaponReward_%d"), i);
            UWeaponReward* WeaponReward = NewObject<UWeaponReward>(this, WeaponClasses[i], *WeaponName);
            if (WeaponReward)
            {
                AvailableWeaponRewards.Add(WeaponReward, WeaponReward->CurrentLevel);
            }
        }
    }
}

void URewardSelectionService::InitializePassiveItemRewards()
{
    AvailablePassiveItemRewards.Empty();
    for (int32 i = 0; i < PassiveItemClasses.Num(); ++i)
    {
        if (PassiveItemClasses[i])
        {
            FString PassiveItemName = FString::Printf(TEXT("PassiveItemReward_%d"), i);
            UPassiveItemReward* PassiveItemReward = NewObject<UPassiveItemReward>(this, PassiveItemClasses[i], *PassiveItemName);
            if (PassiveItemReward)
            {
                AvailablePassiveItemRewards.Add(PassiveItemReward, 0);
            }
        }
    }
}

void URewardSelectionService::InitializePlayerPowerUpRewards()
{
    AvailablePlayerPowerUpRewards.Empty();
    for (int32 i = 0; i < PlayerPowerUpClasses.Num(); ++i)
    {
        if (PlayerPowerUpClasses[i])
        {
            FString PowerUpName = FString::Printf(TEXT("PlayerPowerUpReward_%d"), i);
            UPlayerPowerUpReward* PowerUpReward = NewObject<UPlayerPowerUpReward>(this, PlayerPowerUpClasses[i], *PowerUpName);
            if (PowerUpReward)
            {
                AvailablePlayerPowerUpRewards.Add(PowerUpReward);
            }
        }
    }
}

void URewardSelectionService::UpdateItemPoolWeight()
{
    ItemPoolWeight = 0;
    for (const auto& Pair : AvailablePassiveItemRewards)
    {
        ItemPoolWeight += Pair.Key->Rarity;
    }
}

void URewardSelectionService::HandleWeaponReward(UWeaponReward* WeaponReward)
{
    if (PlayerPawn == nullptr) return;
    
    switch (WeaponReward->WeaponType)
    {
        case EWeaponType::MainWeapon:
            if (AWeaponPawn* MainWeapon = PlayerPawn->GetMainWeapon())
            {
                MainWeapon->WeaponLevelUp(WeaponReward->CurrentLevel);
            }
            break;
        default:
            AWeaponPawn* SubWeapon = PlayerPawn->GetSubWeaponOrNull(WeaponReward->WeaponType);
            if (SubWeapon == nullptr)
            {
                PlayerPawn->AddSubWeapon(WeaponReward->WeaponType);
            }
            else
            {
                SubWeapon->WeaponLevelUp(WeaponReward->CurrentLevel);
            }
            break;
    }
}

void URewardSelectionService::HandlePassiveItemReward(UPassiveItemReward* PassiveItemReward)
{
    if (PlayerPawn == nullptr) return;
    
}

void URewardSelectionService::HandlePlayerPowerUpReward(UPlayerPowerUpReward* PlayerPowerUpReward)
{
    if (PlayerPawn == nullptr) return;

    switch (PlayerPowerUpReward->PowerUpType)
    {
        case EPowerUpType::AdditionalHealth:
            PlayerPawn->SetAdditionalHealth(PlayerPawn->GetAdditionalHealth() + PlayerPowerUpReward->PowerUpValue);
            break;
        case EPowerUpType::DamageScale:
            PlayerPawn->SetDamageScale(PlayerPawn->GetDamageScale() + PlayerPowerUpReward->PowerUpValue);
            break;
        case EPowerUpType::ProjectileSpeedScale:
            PlayerPawn->SetProjectileSpeedScale(PlayerPawn->GetProjectileSpeedScale() + PlayerPowerUpReward->PowerUpValue);
            break;
        case EPowerUpType::AdditionalCritical:
            PlayerPawn->SetAdditionalCritical(PlayerPawn->GetAdditionalCritical() + PlayerPowerUpReward->PowerUpValue);
            break;
        case EPowerUpType::AttackSpeedScale:
            PlayerPawn->SetHasteScale(PlayerPawn->GetHasteScale() + PlayerPowerUpReward->PowerUpValue);
            break;
        case EPowerUpType::XPGainScale:
            PlayerPawn->SetExpGainScale(PlayerPawn->GetExpGainScale() + PlayerPowerUpReward->PowerUpValue);
            break;
        case EPowerUpType::XPSpeedScale:
            PlayerPawn->SetExpSpeedScale(PlayerPawn->GetExpSpeedScale() + PlayerPowerUpReward->PowerUpValue);
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("Unknown PowerUpType: %s"), *PlayerPowerUpReward->Name);
            break;
    }
}

