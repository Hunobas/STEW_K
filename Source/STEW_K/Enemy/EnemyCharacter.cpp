// EnemyCharacter.cpp
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DamageEvents.h"
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

    EnemyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Enemy Mesh"));
	EnemyMesh->SetupAttachment(CapsuleComp);

    AimPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Aim Point"));
    AimPoint->SetupAttachment(EnemyMesh);

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
    PlayerPawn = Cast<APlanetPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	HealthComponent = FindComponentByClass<UHealthComponent>();
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (PlayerPawn && bIsInJustAimWindow)
    {
        if (IsPlayerAimToThis())
        {
            FDamageEvent DamageEvent;
            TakeDamage(JustAimDamage, DamageEvent, GetController(), this);
            PlayerPawn->SucceedJustAim(this);
            
            GetWorldTimerManager().ClearTimer(ShootTimerHandle);
            bIsInJustAimWindow = false;
        }
    }
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
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true
    );

    GetWorldTimerManager().SetTimer(JustAimWindowTimerHandle, this, &AEnemyCharacter::StartJustAimWindow, JustAimDelay, false);
    GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AEnemyCharacter::ExecuteShoot, ShootDelay, false);
}

void AEnemyCharacter::StartJustAimWindow()
{
    bIsInJustAimWindow = true;
    GetWorldTimerManager().SetTimer(JustAimWindowTimerHandle, this, &AEnemyCharacter::EndJustAimWindow, ShootDelay - JustAimDelay - 0.01f, false);
}

void AEnemyCharacter::EndJustAimWindow()
{
    bIsInJustAimWindow = false;
}

void AEnemyCharacter::ExecuteShoot()
{
    if (!bIsInJustAimWindow)
    {
        Shoot();
    }
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

bool AEnemyCharacter::IsPlayerAimToThis() const
{
    FVector DestinationVector = GetActorLocation() - PlayerPawn->GetForwardCelestialVector();
    FVector2D LastAverageAimInput = PlayerPawn->GetLastAverageAimInput();

    if (DestinationVector.Size() < 500.f)
    {
        if (DestinationVector.X <= 0 && DestinationVector.Y <= 0
            && LastAverageAimInput.X <= 0 && LastAverageAimInput.Y <= 0)
        {
            return true;
        }
        else if (DestinationVector.X >= 0 && DestinationVector.Y <= 0
            && LastAverageAimInput.X >= 0 && LastAverageAimInput.Y <= 0)
        {
            return true;
        }
        else if (DestinationVector.X >= 0 && DestinationVector.Y >= 0
            && LastAverageAimInput.X >= 0 && LastAverageAimInput.Y >= 0)
        {
            return true;
        }
        else if (DestinationVector.X <= 0 && DestinationVector.Y >= 0
            && LastAverageAimInput.X <= 0 && LastAverageAimInput.Y >= 0)
        {
            return true;
        }
    }

    return false;
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