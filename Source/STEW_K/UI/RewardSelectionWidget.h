// RewardSelectionWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "RewardSelectionWidget.generated.h"

class URewardSelectionService;
class UButton;

UCLASS()
class STEW_K_API URewardSelectionWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UButton* RewardButton1;
    UPROPERTY(meta = (BindWidget))
    UButton* RewardButton2;
    UPROPERTY(meta = (BindWidget))
    UButton* RewardButton3;

public:
	void DisplayRewards(const TArray<UObject*>& Rewards);

	UPROPERTY()
    URewardSelectionService* RewardService;

private:
    UFUNCTION()
    void OnRewardButton1Clicked();
    UFUNCTION()
    void OnRewardButton2Clicked();
    UFUNCTION()
    void OnRewardButton3Clicked();

	void SetupRewardButton(UButton* Button, UObject* Reward);
	void HandleRewardSelection(int32 ButtonIndex);

    void CloseRewardSelection();

	TArray<UObject*> CurrentRewards;
	
};