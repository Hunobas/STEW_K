// STEWKGameModeBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "STEWKGameModeBase.generated.h"

class AEnemyCharacter;
class APlanetPawn;
class ACelestialBody;
class AXPGem;
class UWaveManager;
class URewardSelectionService;
class UGameTimeManager;

/**
 * 
 */
UCLASS()
class STEW_K_API ASTEWKGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
    ASTEWKGameModeBase();

protected:
	virtual void BeginPlay() override;

public:
    void ActorDied(AActor* DeadActor);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "아웃 게임")
    float StartDelay = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "행성 공전")
    float OrbitRadius = 20000.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "행성 공전")
    float OrbitPeriod = 1200.0f; // 20분

    // ====================== 게터 =============================
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "매니저")
    URewardSelectionService* GetRewardSelectionService() const { return RewardSelectionService; }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "매니저")
    UGameTimeManager* GetGameTimeManager() const { return GameTimeManager; }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "매니저")
    ACelestialBody* GetCelestialBody() const { return CelestialBody; }
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "매니저")
    UWaveManager* GetWaveManager() const { return WaveManager; }

private:
    APlanetPawn* PlayerPawn;
    FTimerHandle GameTimerHandle;
    FTimerHandle EnemyListByDifficultyTimerHandle;
    FTimerHandle FieldScoreByDifficultyTimerHandle;
    float DifficultyTimeInterval = 5.0f;

    UPROPERTY()
    UGameTimeManager* GameTimeManager;

    UPROPERTY(EditDefaultsOnly, Category = "매니저 클래스")
    TSubclassOf<URewardSelectionService> RewardSelectionServiceClass;
    UPROPERTY()
    URewardSelectionService* RewardSelectionService;

    UPROPERTY(EditDefaultsOnly, Category = "매니저 클래스")
    TSubclassOf<ACelestialBody> CelestialBodyClass;
    UPROPERTY()
    ACelestialBody* CelestialBody;

    UPROPERTY(EditDefaultsOnly, Category = "매니저 클래스")
    TSubclassOf<UWaveManager> WaveManagerClass;
    UPROPERTY()
    UWaveManager* WaveManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XP", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AXPGem> XPGem;

    void StartGame();
    void SpawnXpGem(AEnemyCharacter* DestroyedEnemy);
    void EndGame();

    FVector GetRandomSpawnLocation() const;
    void InitializeRewardSelectionService();
    void InitializeCelestialBody();
    void InitializeWaveManager();

};
