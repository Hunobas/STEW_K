// WeaponReward.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "WeaponReward.generated.h"

UENUM(Blueprintable)
enum class EWeaponType : uint8
{
    MainWeapon,
    EquatorWave,
    WeatherStation
};

UCLASS(Blueprintable)
class STEW_K_API UWeaponReward : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EWeaponType WeaponType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString WeaponName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 CurrentLevel = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 MaxLevel = 7;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString DescLV1;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString DescLV2;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString DescLV3;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString DescLV4;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString DescLV5;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString DescLV6;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString DescLV7;

    bool IsMaxLevel() const { return CurrentLevel >= MaxLevel; }
    void SetDescriptionByLevel(const int32& NextLevel);
};
