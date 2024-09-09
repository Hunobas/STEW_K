// PlayerPowerUpReward.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "PlayerPowerUpReward.generated.h"

UENUM(BlueprintType)
enum class EPowerUpType : uint8
{
    AdditionalHealth,
    DamageScale,
    ProjectileSpeedScale,
    AdditionalCritical,
    AttackSpeedScale,
    XPGainScale,
    XPSpeedScale
};

UCLASS(Blueprintable)
class STEW_K_API UPlayerPowerUpReward : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EPowerUpType PowerUpType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString Name;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* Icon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float PowerUpValue = 0.f;
};