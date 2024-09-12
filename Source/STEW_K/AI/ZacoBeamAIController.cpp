// ZacoBeamAIController.cpp
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../Enemy/EnemyCharacter.h"
#include "../Planet/PlanetPawn.h"

#include "ZacoBeamAIController.h"


AZacoBeamAIController::AZacoBeamAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AZacoBeamAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    if (AEnemyCharacter* EnemyPawn = Cast<AEnemyCharacter>(InPawn))
    {
        Speed *= EnemyPawn->GetSpeedScale();
        EnemyPawn->ReadyToShoot();
    }
    // if (ZacoBeamAIBehavior)
    // {
    //     RunBehaviorTree(ZacoBeamAIBehavior);
    // }
}

void AZacoBeamAIController::Tick(float DeltaTime)
{
    // APlanetPawn* PlayerPawn = Cast<APlanetPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    // GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
    AEnemyCharacter* EnemyPawn = Cast<AEnemyCharacter>(GetPawn());
    if (EnemyPawn)
    {
        FVector CurrentLocation = EnemyPawn->GetActorLocation();
        FVector NewLocation = CurrentLocation + FVector(0, 0, FMath::Sin(GetWorld()->GetTimeSeconds() * 2) * 0.1f);
        EnemyPawn->SetActorLocation(NewLocation, true);
    }
}
