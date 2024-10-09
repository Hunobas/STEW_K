// STEWKGameModeBase.cpp
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Planet/PlanetPawn.h"
#include "Enemy/EnemyCharacter.h"
#include "Reward/RewardSelectionService.h"
#include "XPGem.h"
#include "CelestialBody.h"
#include "WaveManager.h"
#include "GameTimeManager.h"

#include "STEWKGameModeBase.h"

ASTEWKGameModeBase::ASTEWKGameModeBase()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASTEWKGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    
    PlayerPawn = Cast<APlanetPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!PlayerPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("Player Pawn not found!"));
        return;
    }

    GameTimeManager = NewObject<UGameTimeManager>(this);
    GameTimeManager->RegisterComponent();
    GameTimeManager->ResetTime();

    FTimerHandle StartDelayHandle;
    GetWorldTimerManager().SetTimer(StartDelayHandle, this, &ASTEWKGameModeBase::StartGame, StartDelay, false);

    InitializeRewardSelectionService();
    InitializeCelestialBody();
    InitializeWaveManager();
}

void ASTEWKGameModeBase::ActorDied(AActor* DeadActor, bool bIsCaught)
{
    if (DeadActor == PlayerPawn)
    {
        if (APlayerController* PlayerController = Cast<APlayerController>(PlayerPawn->GetController()))
        {
            PlayerPawn->DisableInput(PlayerController);
            PlayerController->bShowMouseCursor = true;
        }
        PlayerPawn->HandleDestruction();
    }
    else if (AEnemyCharacter* DestroyedEnemy = Cast<AEnemyCharacter>(DeadActor))
    {
        if (WaveManager)
        {
            WaveManager->SubtractFieldScore(DestroyedEnemy->GetFieldScore());
        }
        if (bIsCaught && FMath::RandRange(0.f, 1.f) <= DestroyedEnemy->GetXPDropScale())
        {
            SpawnXpGem(DestroyedEnemy);
        }
        DestroyedEnemy->HandleDestruction();
    }
}

// 게임 모드 1.
void ASTEWKGameModeBase::StartGame()
{
    // 게임 종료 타이머 설정
    GetWorldTimerManager().SetTimer(GameTimerHandle, this, &ASTEWKGameModeBase::EndGame, OrbitPeriod, false);
    
    GetWorldTimerManager().SetTimer(EnemyListByDifficultyTimerHandle, WaveManager, &UWaveManager::UpdateSpawnableEnemyList, DifficultyTimeInterval, true);
    GetWorldTimerManager().SetTimer(FieldScoreByDifficultyTimerHandle, WaveManager, &UWaveManager::UpdateMaxFieldScore, DifficultyTimeInterval, true);

}

void ASTEWKGameModeBase::SpawnXpGem(AEnemyCharacter* DestroyedEnemy)
{
    if (!XPGem) return;

    FVector SpawnLocation = DestroyedEnemy->GetActorLocation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AXPGem* SpawnedXPGem = GetWorld()->SpawnActor<AXPGem>(XPGem, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
}

void ASTEWKGameModeBase::EndGame()
{
    GetWorldTimerManager().ClearTimer(GameTimerHandle);
    GetWorldTimerManager().ClearTimer(EnemyListByDifficultyTimerHandle);
    GetWorldTimerManager().ClearTimer(FieldScoreByDifficultyTimerHandle);
    UE_LOG(LogTemp, Warning, TEXT("Game Over - Player Wins!"));
}

void ASTEWKGameModeBase::InitializeRewardSelectionService()
{
    if (RewardSelectionServiceClass)
    {
        RewardSelectionService = NewObject<URewardSelectionService>(this, RewardSelectionServiceClass, TEXT("Reward Selection Service"));
        if (RewardSelectionService)
        {
            RewardSelectionService->InitializeRewards();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create RewardSelectionService"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RewardSelectionServiceClass is not set in GameMode"));
    }
}

void ASTEWKGameModeBase::InitializeCelestialBody()
{
    if (CelestialBodyClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        CelestialBody = GetWorld()->SpawnActor<ACelestialBody>(CelestialBodyClass, PlayerPawn->GetActorLocation(), FRotator(0.f, 6.f, 0.f), SpawnParams);

        if (!CelestialBody)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create CelestialBody"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CelestialBodyClass is not set in GameMode"));
    }
}

void ASTEWKGameModeBase::InitializeWaveManager()
{
    if (WaveManagerClass)
    {
        WaveManager = NewObject<UWaveManager>(this, WaveManagerClass, TEXT("Wave Manager"));
        if (WaveManager)
        {
            WaveManager->InitializeWaveManager();
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create WaveManager"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WaveManagerClass is not set in GameMode"));
    }
}