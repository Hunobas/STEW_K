// PassiveItemReward.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "PassiveItemReward.generated.h"

UCLASS(Blueprintable)
class STEW_K_API UPassiveItemReward : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString ItemName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 CurrentLevel = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 MaxLevel = 5;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 Rarity;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FString Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UTexture2D* Icon;

    bool IsMaxLevel() const { return CurrentLevel >= MaxLevel; }

};