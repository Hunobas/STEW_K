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
        if (FMath::RandRange(0.f, 1.f) <= DestroyedEnemy->GetXPDropScale())
        {
            SpawnXpGem(DestroyedEnemy);
        }
        DestroyedEnemy->HandleDestruction();
    }
}

void ASTEWKGameModeBase::StartGame()
{
    // 게임 종료 타이머 설정
    GetWorldTimerManager().SetTimer(GameTimerHandle, this, &ASTEWKGameModeBase::EndGame, OrbitPeriod, false);

    // 적 스폰 타이머 설정
    GetWorldTimerManager().SetTimer(
        ZacoBomb_SpawnTimerHandle,
        FTimerDelegate::CreateUObject(this, &ASTEWKGameModeBase::SpawnEnemies, ZacoBomb, &ZacoBomb_SpawnNumber),
        ZacoBomb_SpawnInterval,
        true
    );
    GetWorldTimerManager().SetTimer(
        ZacoBeam_SpawnTimerHandle,
        FTimerDelegate::CreateUObject(this, &ASTEWKGameModeBase::SpawnEnemies, ZacoBeam, &ZacoBeam_SpawnNumber),
        ZacoBeam_SpawnInterval,
        true
    );
    GetWorldTimerManager().SetTimer(
        ZacoSwarm_SpawnTimerHandle,
        FTimerDelegate::CreateUObject(this, &ASTEWKGameModeBase::SpawnEnemies, ZacoSwarm, &ZacoSwarm_SpawnNumber),
        ZacoSwarm_SpawnInterval,
        true
    );

    // 난이도 증가 타이머 설정
    // GetWorldTimerManager().SetTimer(
    //     ZacoBomb_DifficultyTimerHandle,
    //     FTimerDelegate::CreateUObject(this, &ASTEWKGameModeBase::IncreaseDifficulty, &ZacoBomb_SpawnTimerHandle, &ZacoBomb_SpawnInterval, ZacoBomb, &ZacoBomb_SpawnNumber),
    //     ZacoBomb_IncreaseDifficultyInterval,
    //     true
    // );
    // GetWorldTimerManager().SetTimer(
    //     ZacoSwarm_DifficultyTimerHandle,
    //     FTimerDelegate::CreateUObject(this, &ASTEWKGameModeBase::IncreaseDifficulty, &ZacoSwarm_SpawnTimerHandle, &ZacoSwarm_SpawnInterval, ZacoSwarm, &ZacoSwarm_SpawnNumber),
    //     ZacoSwarm_IncreaseDifficultyInterval,
    //     true
    // );
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

void ASTEWKGameModeBase::EndGame()
{
    GetWorldTimerManager().ClearTimer(ZacoBomb_SpawnTimerHandle);
    GetWorldTimerManager().ClearTimer(ZacoBomb_DifficultyTimerHandle);
    GetWorldTimerManager().ClearTimer(ZacoSwarm_SpawnTimerHandle);
    GetWorldTimerManager().ClearTimer(ZacoSwarm_DifficultyTimerHandle);
    GetWorldTimerManager().ClearTimer(ZacoBeam_SpawnTimerHandle);
    GetWorldTimerManager().ClearTimer(ZacoBeam_DifficultyTimerHandle);
    UE_LOG(LogTemp, Warning, TEXT("Game Over - Player Wins!"));
}

void ASTEWKGameModeBase::SpawnEnemies(TSubclassOf<AEnemyCharacter> EnemyClass, int32* EnemySpawnNumber)
{
    if (CurrentEnemyCount >= MaxEnemies || !EnemyClass) return;

    FVector SpawnPoint = GetRandomSpawnLocation();
    for (int32 SpawnNumber = *EnemySpawnNumber; SpawnNumber > 0; SpawnNumber--)
    {
        float Pitch = FMath::RandRange(0.f, 360.f);
        float Roll = FMath::RandRange(0.f, 360.f);
        float Yaw = FMath::RandRange(0.f, 360.f);
        FVector SpawnOffset = FRotator(Pitch, Roll, Yaw).Vector() * FMath::RandRange(0.f, 250.f);

        FVector SpawnLocation = SpawnPoint + SpawnOffset;
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        if (SpawnedEnemy)
        {
            CurrentEnemyCount++;
        }
    }
}

// void ASTEWKGameModeBase::IncreaseDifficulty(FTimerHandle* SpawnTimerHandle, float* SpawnInterval, TSubclassOf<AEnemyCharacter> EnemyClass, int32* EnemySpawnNumber)
// {
//     if (SpawnTimerHandle && SpawnInterval)
//     {
//         *SpawnInterval = FMath::Max(*SpawnInterval * 0.9f, 0.5f);
//         GetWorldTimerManager().SetTimer(*SpawnTimerHandle, this, SpawnFunction, *SpawnInterval, true); 

//         // TEMP
//         SpawnEnemies(EnemyClass, EnemySpawnNumber);
//         HealthScale += 0.05f;
//         DamageScale += 0.05f;
//         SpeedScale += 0.05f;
//     }
// }

FVector ASTEWKGameModeBase::GetRandomSpawnLocation() const
{
    if (!PlayerPawn)
        return FVector::ZeroVector;

    FVector PlayerLocation = PlayerPawn->GetActorLocation();
    FVector RandomDirection = FMath::VRand();
    RandomDirection.Z = FMath::Clamp(RandomDirection.Z, -0.18, 0.18);
    RandomDirection.Normalize();

    return PlayerLocation + RandomDirection * FMath::RandRange(SpawnRadius * 0.8f, SpawnRadius);
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
