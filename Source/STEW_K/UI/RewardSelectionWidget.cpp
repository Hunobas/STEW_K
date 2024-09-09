// RewardSelectionWidget.cpp
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"

#include "../STEWKGameModeBase.h"
#include "../Planet/PlanetPlayerController.h"
#include "../Reward/RewardSelectionService.h"
#include "../Reward/WeaponReward.h"
#include "../Reward/PassiveItemReward.h"
#include "../Reward/PlayerPowerUpReward.h"

#include "RewardSelectionWidget.h"


void URewardSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ASTEWKGameModeBase* GameMode = Cast<ASTEWKGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        RewardService = GameMode->GetRewardSelectionService();
        if (RewardService)
        {
            TArray<UObject*> SelectedRewards = RewardService->GetRandomRewards();
            DisplayRewards(SelectedRewards);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("RewardService is not initialized in GameMode"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get STEWKGameModeBase"));
    }

    if (RewardButton1) RewardButton1->OnClicked.AddDynamic(this, &URewardSelectionWidget::OnRewardButton1Clicked);
    if (RewardButton2) RewardButton2->OnClicked.AddDynamic(this, &URewardSelectionWidget::OnRewardButton2Clicked);
    if (RewardButton3) RewardButton3->OnClicked.AddDynamic(this, &URewardSelectionWidget::OnRewardButton3Clicked);
}

void URewardSelectionWidget::DisplayRewards(const TArray<UObject*>& Rewards)
{
    CurrentRewards = Rewards;

    TArray<UButton*> Buttons = { RewardButton1, RewardButton2, RewardButton3 };

    for (int32 i = 0; i < FMath::Min(Buttons.Num(), Rewards.Num()); ++i)
    {
        SetupRewardButton(Buttons[i], Rewards[i]);
    }
}

void URewardSelectionWidget::OnRewardButton1Clicked()
{
    HandleRewardSelection(0);
}

void URewardSelectionWidget::OnRewardButton2Clicked()
{
    HandleRewardSelection(1);
}

void URewardSelectionWidget::OnRewardButton3Clicked()
{
    HandleRewardSelection(2);
}

void URewardSelectionWidget::SetupRewardButton(UButton* Button, UObject* Reward)
{
    if (!Button || !Reward) return;

    UPanelWidget* ButtonPanel = Cast<UPanelWidget>(Button->GetChildAt(0));
    if (!ButtonPanel) return;

    UImage* IconImage = Cast<UImage>(ButtonPanel->GetChildAt(0));
    UPanelWidget* TextPanel = Cast<UPanelWidget>(ButtonPanel->GetChildAt(1));
    
    if (!TextPanel) return;

    UTextBlock* NameText = Cast<UTextBlock>(TextPanel->GetChildAt(0));
    UTextBlock* DescriptionText = Cast<UTextBlock>(TextPanel->GetChildAt(1));

    FString RewardName;
    FString RewardDescription;
    UTexture2D* RewardIcon = nullptr;

    if (UWeaponReward* WeaponReward = Cast<UWeaponReward>(Reward))
    {
        RewardName = WeaponReward->WeaponName;
        RewardDescription = WeaponReward->Description;
        RewardIcon = WeaponReward->Icon;
    }
    else if (UPassiveItemReward* PassiveItemReward = Cast<UPassiveItemReward>(Reward))
    {
        RewardName = PassiveItemReward->ItemName;
        RewardDescription = PassiveItemReward->Description;
        RewardIcon = PassiveItemReward->Icon;
    }
    else if (UPlayerPowerUpReward* PowerUpReward = Cast<UPlayerPowerUpReward>(Reward))
    {
        RewardName = PowerUpReward->Name;
        RewardDescription = PowerUpReward->Description;
        RewardIcon = PowerUpReward->Icon;
    }

    if (IconImage) IconImage->SetBrushFromTexture(RewardIcon);
    if (NameText) NameText->SetText(FText::FromString(RewardName));
    if (DescriptionText) DescriptionText->SetText(FText::FromString(RewardDescription));

    Button->SetVisibility(ESlateVisibility::Visible);
}

void URewardSelectionWidget::HandleRewardSelection(int32 ButtonIndex)
{
    if (CurrentRewards.IsValidIndex(ButtonIndex))
    {
        UObject* SelectedReward = CurrentRewards[ButtonIndex];
        RewardService->AcquireReward(SelectedReward);
    }

    CloseRewardSelection();
}

void URewardSelectionWidget::CloseRewardSelection()
{
    RemoveFromParent();
    
    if (APlanetPlayerController* PC = Cast<APlanetPlayerController>(GetOwningPlayer()))
    {
        PC->ResumeGameplay();
    }
}