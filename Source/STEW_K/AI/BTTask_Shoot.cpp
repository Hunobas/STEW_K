// BTTask_Shoot.cpp
#include "AIController.h"

#include "../Enemy/EnemyCharacter.h"

#include "BTTask_Shoot.h"


UBTTask_Shoot::UBTTask_Shoot()
{
    NodeName = "Shoot";
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    if (OwnerComp.GetAIOwner() == nullptr)
    {
        return EBTNodeResult::Failed;
    }
    if (AEnemyCharacter* ControlledPawn = Cast<AEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn()))
    {
        ControlledPawn->ReadyToShoot();
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}