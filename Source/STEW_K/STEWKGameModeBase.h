// STEWKGameModeBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "STEWKGameModeBase.generated.h"

class AEnemyCharacter;
class APlanetPawn;
class ACelestialBody;
class AXPGem;
class URewardSelectionService;

/**
 * 
 */
UCLASS()
class STEW_K_API ASTEWKGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
    ASTEWKGameModeBase();

    UFUNCTION(BlueprintCallable, Category = "Rewards")
    URewardSelectionService* GetRewardSelectionService() const { return RewardSelectionService; }

protected:
	virtual void BeginPlay() override;

public:
    void ActorDied(AActor* DeadActor);
    void GainXpGem();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "아웃 게임")
    float StartDelay = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "행성 공전")
    float OrbitRadius = 20000.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "행성 공전")
    float OrbitPeriod = 1200.0f; // 20분

    // ====================== 게터 =============================
	float GetHealthScale() const { return HealthScale; }
	float GetDamageScale() const { return DamageScale; }
	float GetSpeedScale() const { return SpeedScale; }

private:
    // ====================== 몬스터 =============================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AEnemyCharacter> ZacoBomb;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    int32 ZacoBomb_SpawnNumber = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    float ZacoBomb_SpawnInterval = 5.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    float ZacoBomb_IncreaseDifficultyInterval = 60.f;

    FTimerHandle ZacoBomb_SpawnTimerHandle;
    FTimerHandle ZacoBomb_DifficultyTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AEnemyCharacter> ZacoSwarm;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    int32 ZacoSwarm_SpawnNumber = 10;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    float ZacoSwarm_SpawnInterval = 60.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    float ZacoSwarm_IncreaseDifficultyInterval = 180.f;

    FTimerHandle ZacoSwarm_SpawnTimerHandle;
    FTimerHandle ZacoSwarm_DifficultyTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AEnemyCharacter> ZacoBeam;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    int32 ZacoBeam_SpawnNumber = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    float ZacoBeam_SpawnInterval = 40.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    float ZacoBeam_IncreaseDifficultyInterval = 60.f;

    FTimerHandle ZacoBeam_SpawnTimerHandle;
    FTimerHandle ZacoBeam_DifficultyTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    float SpawnRadius = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    int32 MaxEnemies = 50;
    UPROPERTY(EditDefaultsOnly, Category = "몬스터")
	float HealthScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "몬스터")
	float DamageScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "몬스터")
	float SpeedScale = 1.f;
    // ========================================================

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XP", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AXPGem> XPGem;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XP", meta = (AllowPrivateAccess = "true"))
    int32 MaxXPGems = 40;

    UPROPERTY(EditDefaultsOnly, Category = "리워드")
    TSubclassOf<URewardSelectionService> RewardSelectionServiceClass;
    UPROPERTY()
    URewardSelectionService* RewardSelectionService;

    UPROPERTY(EditDefaultsOnly, Category = "클래스")
    TSubclassOf<ACelestialBody> CelestialBodyClass;
    UPROPERTY()
    ACelestialBody* CelestialBody;

    FTimerHandle GameTimerHandle;
    int32 CurrentEnemyCount = 0;
    int32 CurrentXpGemCount = 0;
    float ElapsedTime = 0.f;
    APlanetPawn* PlayerPawn;

    void StartGame();
    void SpawnXpGem(AEnemyCharacter* DestroyedEnemy);
    void EndGame();
    void SpawnEnemies(TSubclassOf<AEnemyCharacter> EnemyClass, int32* EnemySpawnNumber);
    void SpawnZacoBeam();
    void OnZacoBeamDestroyed(AActor* DestroyedActor);

    typedef void (ASTEWKGameModeBase::*SpawnFunctionPtr)();
    void IncreaseDifficulty(FTimerHandle* SpawnTimerHandle, float* SpawnInterval, TSubclassOf<AEnemyCharacter> EnemyClass, int32* EnemySpawnNumber);

    FVector GetRandomSpawnLocation() const;
    void InitializeRewardSelectionService();
    void InitializeCelestialBody();

};
