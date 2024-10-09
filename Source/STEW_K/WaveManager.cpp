// WaveManager.cpp
#include "Kismet/GameplayStatics.h"

#include "STEWKGameModeBase.h"
#include "GameTimeManager.h"
#include "Enemy/EnemyCharacter.h"
#include "CelestialBody.h"

#include "WaveManager.h"

void UWaveManager::InitializeWaveManager()
{
    if (ASTEWKGameModeBase* GameMode = Cast<ASTEWKGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        GameTimeManager = GameMode->GetGameTimeManager();
        CelestialBody = GameMode->GetCelestialBody();
    }
    SpawnableEnemyList.AddUnique(ZacoBomb);
}

void UWaveManager::UpdateMaxFieldScore()
{
    float ElapsedTime = GameTimeManager->GetElapsedTime();
    float SigmoidScore = static_cast<float>(StartScore) + static_cast<float>(EndScore - StartScore) / (1 + FMath::Exp(-Inclination * (ElapsedTime - InflectionPoint)));
    MaxFieldScore = FMath::FloorToInt(SigmoidScore);
    
    SpawnEnemyWave();
}

void UWaveManager::UpdateSpawnableEnemyList()
{
    float ElapsedTime = GameTimeManager->GetElapsedTime();
    if (ElapsedTime > 120.0f)
    {
        SpawnableEnemyList.AddUnique(ZacoBeam);
    }
    else if (ElapsedTime > 90.0f)
    {
        SpawnableEnemyList.AddUnique(ZacoSwarm);
    }
}

void UWaveManager::SpawnEnemyWave()
{
    if (SpawnableEnemyList.IsEmpty() || !CelestialBody || CurrentFieldScore > MaxFieldScore) return;

    int32 SpawnLimit = FMath::Max(50, (MaxFieldScore - CurrentFieldScore) / 5);
    int32 SpawnCount = 0;

    while (CurrentFieldScore <= MaxFieldScore && SpawnCount < SpawnLimit)
    {
        SpawnEnemyAtRandomSpot(SpawnableEnemyList[FMath::RandRange(0, SpawnableEnemyList.Num() - 1)]);
        SpawnCount++;
    }
    CelestialBody->FreeAllAimPoints();

    UE_LOG(LogTemp, Log, TEXT("SpawnEnemyWave spawned %d enemies"), SpawnCount);
}

void UWaveManager::SpawnEnemyAtRandomSpot(TSubclassOf<AEnemyCharacter> EnemyClass)
{
    if (!EnemyClass || !CelestialBody) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    USceneComponent* SpawnPoint = CelestialBody->GetRandomActiveAimPointOrNull();
    if (!SpawnPoint)
    {
        UE_LOG(LogTemp, Warning, TEXT("Warning: CB can't get any spawn point"));
        return;
    }

    AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation(), SpawnParams);
    if (SpawnedEnemy)
    {
        CurrentFieldScore += FMath::Max(1, SpawnedEnemy->GetFieldScore());
        CelestialBody->MarkAimPointAsOccupied(SpawnPoint);
        if (SpawnedEnemy->IsA(ZacoBeam))
        {
            SpawnedEnemy->AttachToComponent(SpawnPoint, FAttachmentTransformRules::KeepWorldTransform);
        }
    }
}

void UWaveManager::SpawnEnemiesAtNthRow(TSubclassOf<AEnemyCharacter> EnemyClass, int32 n)
{
    if (!EnemyClass || !CelestialBody) return;

    TArray<USceneComponent*> SpawnPoints = CelestialBody->GetNthPointsRowOrNull(n);
    if (SpawnPoints.IsEmpty()) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    for (USceneComponent* SpawnPoint : SpawnPoints)
    {
        AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation(), SpawnParams);
        if (SpawnedEnemy)
        {
            CurrentFieldScore += SpawnedEnemy->GetFieldScore();
        }
    }
}

void UWaveManager::SubtractFieldScore(int32 Score)
{
    CurrentFieldScore -= Score;
    UE_LOG(LogTemp, Warning, TEXT("Inside SubtractFieldScore: -%d: %d / %d"), Score, CurrentFieldScore, MaxFieldScore);
}