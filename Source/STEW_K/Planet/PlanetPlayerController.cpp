// PlanetPlayerController.cpp
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "../UI/RewardSelectionWidget.h"
#include "../GameTimeManager.h"

#include "PlanetPlayerController.h"


APlanetPlayerController::APlanetPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void APlanetPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    if(UUserWidget* HUD = CreateWidget(this, HUDClass))
    {
        HUD->AddToViewport();
    }

    GameTimeManager = UGameTimeManager::GetInstance();
}

void APlanetPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

FString APlanetPlayerController::GetTimerString() const
{
    float TotalSeconds = GameTimeManager->GetElapsedTime();
    int32 Minutes = FMath::FloorToInt(TotalSeconds / 60.0f);
    int32 Seconds = FMath::FloorToInt(FMath::Fmod(TotalSeconds, 60.0f));

    return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

void APlanetPlayerController::ShowRewardSelection()
{
    if (RewardSelectionClass)
    {
        UGameplayStatics::SetGamePaused(GetWorld(), true);
        GameTimeManager->PauseTime();
        
        RewardSelectionWidget = CreateWidget<URewardSelectionWidget>(this, RewardSelectionClass);
        if (RewardSelectionWidget)
        {
            RewardSelectionWidget->AddToViewport();
            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());
        }
    }
}

void APlanetPlayerController::ResumeGameplay()
{
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    GameTimeManager->ResumeTime();
    bShowMouseCursor = false;
    SetInputMode(FInputModeGameOnly());
}