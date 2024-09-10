// ZacoBombAIController.cpp
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

#include "../Planet/PlanetPawn.h"
#include "../Enemy/EnemyCharacter.h"
#include "../XPGem.h"

#include "ZacoBombAIController.h"


AZacoBombAIController::AZacoBombAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AZacoBombAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    PlayerPawn = Cast<APlanetPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
    
    if (AEnemyCharacter* EnemyPawn = Cast<AEnemyCharacter>(InPawn))
    {
        Speed *= EnemyPawn->GetSpeedScale();
    }
    if (AXPGem* XPGemPawn = Cast<AXPGem>(InPawn))
    {
        Speed *= XPGemPawn->GetSpeedScale();
    }
}

void AZacoBombAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerPawn)
    {
        MoveTowardsPlayer(DeltaTime);
    }
}

void AZacoBombAIController::MoveTowardsPlayer(float DeltaTime)
{
    if (APawn* ControlledPawn = GetPawn())
    {
        FVector Direction = (PlayerPawn->GetActorLocation() - ControlledPawn->GetActorLocation()).GetSafeNormal();
        FVector NewLocation = ControlledPawn->GetActorLocation() + Direction * Speed * DeltaTime;
        ControlledPawn->SetActorRotation(Direction.Rotation());
        ControlledPawn->SetActorLocation(NewLocation, true);
    }
}