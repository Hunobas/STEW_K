// PlanetPlayerController.cpp
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "../UI/RewardSelectionWidget.h"

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
}

void APlanetPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!bIsGamePaused)
    {
        UpdateTimer(DeltaTime);
    }
}

FString APlanetPlayerController::GetTimerString() const
{
    int32 Minutes = FMath::FloorToInt(TotalSeconds / 60.0f);
    int32 Seconds = FMath::FloorToInt(FMath::Fmod(TotalSeconds, 60.0f));

    return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

void APlanetPlayerController::ShowRewardSelection()
{
    if (RewardSelectionClass)
    {
        bIsGamePaused = true;
        UGameplayStatics::SetGamePaused(GetWorld(), true);
        
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
    bIsGamePaused = false;
    UGameplayStatics::SetGamePaused(GetWorld(), false);
    bShowMouseCursor = false;
    SetInputMode(FInputModeGameOnly());
}

void APlanetPlayerController::UpdateTimer(float DeltaTime)
{
    TotalSeconds += DeltaTime;
}
