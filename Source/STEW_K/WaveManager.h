// WaveManager.h
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WaveManager.generated.h"

class AEnemyCharacter;
class ACelestialBody;
class UGameTimeManager;

/**
 * 
 */
UCLASS(Blueprintable)
class STEW_K_API UWaveManager : public UObject
{
    GENERATED_BODY()

public:
	void InitializeWaveManager();

    UFUNCTION(BlueprintCallable, Category = "Wave Management")
    void UpdateMaxFieldScore();
    UFUNCTION(BlueprintCallable, Category = "Wave Management")
    void UpdateSpawnableEnemyList();

    UFUNCTION(BlueprintCallable, Category = "Wave Management")
    void SpawnEnemyWave();
    UFUNCTION(BlueprintCallable, Category = "Wave Management")
    void SpawnEnemiesAtNthRow(TSubclassOf<AEnemyCharacter> EnemyClass, int32 n);
    UFUNCTION(BlueprintCallable, Category = "Wave Management")
    void SubtractFieldScore(AEnemyCharacter* DestroyedEnemy);

    // ====================== 게터 =============================
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "전투 스케일")
	float GetHealthScale() const { return HealthScale; }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "전투 스케일")
	float GetDamageScale() const { return DamageScale; }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "전투 스케일")
	float GetSpeedScale() const { return SpeedScale; }

private:
    void SpawnEnemyAtRandomSpot(TSubclassOf<AEnemyCharacter> EnemyClass);

	// ====================== 몬스터 =============================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    TArray<TSubclassOf<AEnemyCharacter>> SpawnableEnemyList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AEnemyCharacter> ZacoBomb;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AEnemyCharacter> ZacoSwarm;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AEnemyCharacter> ZacoBeam;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "난이도 강화", meta = (AllowPrivateAccess = "true"))
    int32 MaxFieldScore = 5;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "난이도 강화", meta = (AllowPrivateAccess = "true"))
	int32 StartScore = 4;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "난이도 강화", meta = (AllowPrivateAccess = "true"))
    int32 EndScore = 600;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "난이도 강화", meta = (AllowPrivateAccess = "true"))
    float InflectionPoint = 600.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "난이도 강화", meta = (AllowPrivateAccess = "true"))
    float Inclination = 0.008f;

    UPROPERTY(EditDefaultsOnly, Category = "몬스터")
	float HealthScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "몬스터")
	float DamageScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "몬스터")
	float SpeedScale = 1.f;
    // ========================================================
    UPROPERTY()
    UGameTimeManager* GameTimeManager;
    UPROPERTY()
    ACelestialBody* CelestialBody;

	int32 CurrentFieldScore = 0;

};