// BTTask_FlyToPlayer.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "BTTask_FlyToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class STEW_K_API UBTTask_FlyToPlayer : public UBTTask_BlackboardBase
{
    GENERATED_BODY()

public:
    UBTTask_FlyToPlayer();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = Blackboard)
    FBlackboardKeySelector AcceptableRadiusKey;

private:
    FVector TargetLocation;
	
};
