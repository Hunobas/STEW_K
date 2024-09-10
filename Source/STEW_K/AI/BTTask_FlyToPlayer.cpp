// BTTask_FlyToPlayer.cpp
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ZacoBeamAIController.h"

#include "BTTask_FlyToPlayer.h"

UBTTask_FlyToPlayer::UBTTask_FlyToPlayer()
{
    NodeName = TEXT("Fly To");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FlyToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    return EBTNodeResult::InProgress;
}

void UBTTask_FlyToPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!AIController || !BlackboardComp)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    TargetLocation = BlackboardComp->GetValueAsVector(GetSelectedBlackboardKey());

    APawn* ControlledPawn = AIController->GetPawn();
    if (!ControlledPawn)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AZacoBeamAIController* ZacoBeamAIController = Cast<AZacoBeamAIController>(AIController);
    if (!ZacoBeamAIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    float AcceptableRadius = BlackboardComp->GetValueAsFloat(AcceptableRadiusKey.SelectedKeyName);

    FVector CurrentLocation = ControlledPawn->GetActorLocation();
    FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
    float DistanceToTarget = FVector::Distance(CurrentLocation, TargetLocation);

    if (DistanceToTarget > AcceptableRadius)
    {
        FVector NewLocation = CurrentLocation + Direction * ZacoBeamAIController->GetSpeed() * DeltaSeconds;
        ControlledPawn->SetActorRotation(Direction.Rotation());
        ControlledPawn->SetActorLocation(NewLocation, true);
    }
    else
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}