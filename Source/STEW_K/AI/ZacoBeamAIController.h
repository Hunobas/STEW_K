// ZacoBeamAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "ZacoBeamAIController.generated.h"

class UBehaviorTree;

/**
 * 
 */
UCLASS()
class STEW_K_API AZacoBeamAIController : public AAIController
{
	GENERATED_BODY()

public:
	AZacoBeamAIController();

	float GetSpeed() const { return Speed; }

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	virtual void Tick(float DeltaTime) override;
	
private:
    UPROPERTY(EditDefaultsOnly, Category = "Components")
	UBehaviorTree* ZacoBeamAIBehavior;

    UPROPERTY(EditDefaultsOnly, Category = "적")
    float Speed = 200.0f;

    FTimerHandle FireTimerHandle;

};
