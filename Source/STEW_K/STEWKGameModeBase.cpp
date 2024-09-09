// STEWKGameModeBase.cpp
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Planet/PlanetPawn.h"
#include "Enemy/EnemyCharacter.h"
#include "Reward/RewardSelectionService.h"
#include "XPGem.h"

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

    FTimerHandle StartDelayHandle;
    GetWorldTimerManager().SetTimer(StartDelayHandle, this, &ASTEWKGameModeBase::StartGame, StartDelay, false);

    InitializeRewardSelectionService();
}

void ASTEWKGameModeBase::ActorDied(AActor* DeadActor)
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
        CurrentEnemyCount--;
        SpawnXpGem(DestroyedEnemy);
        DestroyedEnemy->HandleDestruction();
    }
}

void ASTEWKGameModeBase::StartGame()
{
    // 적 스폰 타이머 설정
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASTEWKGameModeBase::SpawnEnemy, SpawnInterval, true);

    // 게임 종료 타이머 설정
    GetWorldTimerManager().SetTimer(GameTimerHandle, this, &ASTEWKGameModeBase::EndGame, OrbitPeriod, false);

    // 난이도 증가 타이머 설정
    GetWorldTimerManager().SetTimer(DifficultyTimerHandle, this, &ASTEWKGameModeBase::IncreaseSpawnRate, IncreaseDifficultyInterval, true);
}

void ASTEWKGameModeBase::SpawnEnemy()
{
    if (CurrentEnemyCount >= MaxEnemies || !ZacoBomb) return;

    FVector SpawnLocation = GetRandomSpawnLocation();

    AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(ZacoBomb, SpawnLocation, FRotator::ZeroRotator);
    if (SpawnedEnemy)
    {
        SpawnedEnemy->SetHealthScale(HealthScale);
        SpawnedEnemy->SetDamageScale(DamageScale);
        SpawnedEnemy->SetSpeedScale(SpeedScale);
        CurrentEnemyCount++;
    }
}

void ASTEWKGameModeBase::SpawnXpGem(AEnemyCharacter* DestroyedEnemy)
{
    if (CurrentXpGemCount >= MaxXPGems || !XPGem) return;

    FVector SpawnLocation = DestroyedEnemy->GetActorLocation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AXPGem* SpawnedXPGem = GetWorld()->SpawnActor<AXPGem>(XPGem, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    if (SpawnedXPGem)
    {
        CurrentXpGemCount++;
    }
}

void ASTEWKGameModeBase::GainXpGem()
{
    CurrentXpGemCount--;
}

void ASTEWKGameModeBase::IncreaseSpawnRate()
{
    SpawnInterval = FMath::Max(SpawnInterval * 0.9f, 0.5f);
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASTEWKGameModeBase::SpawnEnemy, SpawnInterval, true); 

    // TEMP
    HealthScale += 0.05f;
    DamageScale += 0.05f;
    SpeedScale += 0.05f;
}

void ASTEWKGameModeBase::EndGame()
{
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
    GetWorldTimerManager().ClearTimer(DifficultyTimerHandle);
    UE_LOG(LogTemp, Warning, TEXT("Game Over - Player Wins!"));
}

FVector ASTEWKGameModeBase::GetRandomSpawnLocation() const
{
    if (!PlayerPawn)
        return FVector::ZeroVector;

    FVector PlayerLocation = PlayerPawn->GetActorLocation();
    FVector RandomDirection = FMath::VRand();
    RandomDirection.Z = FMath::Clamp(RandomDirection.Z, -0.2, 0.2);
    RandomDirection.Normalize();

    return PlayerLocation + RandomDirection * FMath::RandRange(SpawnRadius * 2/3, SpawnRadius);
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
