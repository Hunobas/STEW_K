// STEWKGameModeBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "STEWKGameModeBase.generated.h"

class AEnemyCharacter;
class APlanetPawn;
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
    float StartDelay = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "행성 공전")
    float OrbitRadius = 20000.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "행성 공전")
    float OrbitPeriod = 1200.0f; // 20분

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AEnemyCharacter> ZacoBomb;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    float SpawnInterval = 5.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "몬스터", meta = (AllowPrivateAccess = "true"))
    float IncreaseDifficultyInterval = 60.f;
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XP", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AXPGem> XPGem;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XP", meta = (AllowPrivateAccess = "true"))
    int32 MaxXPGems = 40;

    UPROPERTY(EditDefaultsOnly, Category = "리워드")
    TSubclassOf<URewardSelectionService> RewardSelectionServiceClass;
    UPROPERTY()
    URewardSelectionService* RewardSelectionService;

    FTimerHandle SpawnTimerHandle;
    FTimerHandle GameTimerHandle;
    FTimerHandle DifficultyTimerHandle;
    int32 CurrentEnemyCount = 0;
    int32 CurrentXpGemCount = 0;
    float ElapsedTime = 0.f;
    APlanetPawn* PlayerPawn;

    void StartGame();
    void SpawnEnemy();
    void SpawnXpGem(AEnemyCharacter* DestroyedEnemy);
    void IncreaseSpawnRate();
    void EndGame();

    FVector GetRandomSpawnLocation() const;
    void InitializeRewardSelectionService();

};
