// EnemyCharacter.cpp
#include "GameFramework\CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "../HealthComponent.h"

#include "EnemyCharacter.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::Spawned;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

    if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
    {
        MovementComponent->GravityScale = 0.0f;
        MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
        MovementComponent->DefaultLandMovementMode = EMovementMode::MOVE_Flying;
        MovementComponent->DefaultWaterMovementMode = EMovementMode::MOVE_Flying;
    }
	HealthComponent = FindComponentByClass<UHealthComponent>();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemyCharacter::GetHealthPercentage() const
{
	if (HealthComponent && HealthComponent->GetMaxHealth() > 0)
    {
        return HealthComponent->GetHealth() / HealthComponent->GetMaxHealth();
    }
    return 0.0f;
}

void AEnemyCharacter::HandleDestruction()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		DestructionTemplate,
		GetActorLocation(),
		GetActorRotation()
	);
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		DestructionSound,
		GetActorLocation()
	);
	Destroy();
}