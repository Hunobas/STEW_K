// ZacoBombAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "ZacoBombAIController.generated.h"

class APlanetPawn;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class STEW_K_API AZacoBombAIController : public AAIController
{
	GENERATED_BODY()

public:
	AZacoBombAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	virtual void Tick(float DeltaTime) override;

private: 
    void MoveTowardsPlayer(float DeltaTime);

    UPROPERTY(EditDefaultsOnly, Category = "적")
    float Speed = 100.0f;

    APlanetPawn* PlayerPawn;
};
