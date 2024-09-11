// EnemyCharacter.cpp
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "../Planet/PlanetPawn.h"
#include "../STEWKGameModeBase.h"
#include "../HealthComponent.h"

#include "EnemyCharacter.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
    RootComponent = CapsuleComp;

    AimPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Aim Point"));
    AimPoint->SetupAttachment(CapsuleComp);

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
    if (ASTEWKGameModeBase* GameMode = Cast<ASTEWKGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		SetHealthScale(GameMode->GetHealthScale());
        SetDamageScale(GameMode->GetDamageScale());
        SetSpeedScale(GameMode->GetSpeedScale());
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

void AEnemyCharacter::ReadyToShoot()
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(
        JustAimTemplate,
        AimPoint,
        NAME_None,
        FVector::ZeroVector,
        AimPoint->GetComponentRotation(),
        EAttachLocation::SnapToTarget,
        true
    );

	FTimerHandle ShootTimerHandle;
	GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AEnemyCharacter::Shoot, JustAim, false);
}

void AEnemyCharacter::Shoot()
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(
        ShotMuzzleTemplate,
        AimPoint,
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true
    );
    UGameplayStatics::PlaySoundAtLocation(
        GetWorld(),
        ShotSound,
        AimPoint->GetComponentLocation()
    );
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