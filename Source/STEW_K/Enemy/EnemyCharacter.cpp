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
        const FHitResult* HitResult = GetPlayerAimHitResult();
        if (HitResult)
        {
            FDamageEvent DamageEvent;
            TakeDamage(40.0f, DamageEvent, GetController(), this);
            PlayerPawn->SucceedJustAim(*HitResult);
            
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

const FHitResult* AEnemyCharacter::GetPlayerAimHitResult() const
{
    if (!PlayerPawn)
    {
        return nullptr;
    }

    FCollisionShape SweepShape = FCollisionShape::MakeSphere(JustAimAcceptableRadius);

    FVector SweepStart = PlayerPawn->GetSweepStartLocation();
    FVector SweepEnd = PlayerPawn->GetSweepEndLocation();

    TArray<FHitResult> HitResults;
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        SweepStart,
        SweepEnd,
        FQuat::Identity,
        ECC_GameTraceChannel4,
        SweepShape
    );

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            if (Hit.GetActor() == this)
            {
                return &Hit;
            }
        }
    }

    return nullptr;
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