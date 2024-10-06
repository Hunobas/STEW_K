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
    UE_LOG(LogTemp, Warning, TEXT("Inside SpawnEnemyWave: %d / %d"), CurrentFieldScore, MaxFieldScore);
    if (SpawnableEnemyList.IsEmpty() || CurrentFieldScore > MaxFieldScore) return;

    while (CurrentFieldScore <= MaxFieldScore)
    {
        SpawnEnemyAtRandomSpot(SpawnableEnemyList[FMath::RandRange(0, SpawnableEnemyList.Num() - 1)]);
    }
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
        if (SpawnedEnemy->IsA(ZacoBeam))
        {
            CelestialBody->MarkAimPointAsOccupied(SpawnPoint);
            SpawnedEnemy->AttachToComponent(SpawnPoint, FAttachmentTransformRules::KeepWorldTransform);
        }
    }
}

void UWaveManager::SpawnEnemiesAtNthRow(TSubclassOf<AEnemyCharacter> EnemyClass, int32 n)
{
    if (CurrentFieldScore >= MaxFieldScore || !EnemyClass || !CelestialBody) return;

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

void UWaveManager::SubtractFieldScore(AEnemyCharacter* DestroyedEnemy)
{
    CurrentFieldScore -= DestroyedEnemy->GetFieldScore();
    UE_LOG(LogTemp, Warning, TEXT("Inside SubtractFieldScore: %d"), CurrentFieldScore);
}