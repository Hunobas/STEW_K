// PlanetPlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "PlanetPlayerController.generated.h"

class UUserWidget;
class URewardSelectionWidget;
class UGameTimeManager;

/**
 * 
 */
UCLASS()
class STEW_K_API APlanetPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    APlanetPlayerController();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    FString GetTimerString() const;

    void ShowRewardSelection();
    void ResumeGameplay();

    void ShowWinGame();
    void ShowLoseGame();

private:
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> HUDClass;
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> WinGameClass;
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> LoseGameClass;
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<URewardSelectionWidget> RewardSelectionClass;
    UPROPERTY()
    URewardSelectionWidget* RewardSelectionWidget;

    UPROPERTY()
    UGameTimeManager* GameTimeManager;

};