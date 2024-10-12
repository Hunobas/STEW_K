// PlanetPawn.cpp
// Engine Component
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
// EnhancedInput
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
// Custom Header
#include "../STEWKGameModeBase.h"
#include "PlanetPlayerController.h"
#include "../Weapon/WeaponPawn.h"
#include "../HealthComponent.h"
#include "../Enemy/EnemyCharacter.h"

#include "PlanetPawn.h"

// Sets default values
APlanetPawn::APlanetPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ComposeBaseSubobject();
	ComposeWeaponSubobject();
	InitializeWeaponSockets();

	SpringArm->TargetArmLength = CurrentArmLength = DefaultArmLength;
	bIsAiming = false;
	bIsSnapLook = false;

}

// Called when the game starts or when spawned
void APlanetPawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
	
	CurrentAngle = 0.0f;
    CumulativeRotation = 0.0f;
    CumulativeCorrection = CumulativeDefaultCorrection;
	RotationRate = DefaultRotationRate;
	LastSnappedLookTime = -SnappedLookCooldown;
	SnappedRotation = GetActorRotation();
    
	UpdateOrbitParameters();
    SpawnMainWeapon();

	CurrentLevel = 1;
    CurrentXP = 0;
    CalculateXPToNextLevel();

    HealthComponent = FindComponentByClass<UHealthComponent>();

}

// Called every frame
void APlanetPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunOrbit(DeltaTime);
	UpdateArmLength(DeltaTime);
}

// Called to bind functionality to input
void APlanetPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlanetPawn::Look);
        EnhancedInputComponent->BindAction(MouseLeftAction, ETriggerEvent::Triggered, this, &APlanetPawn::TriggerSnappedLook);
        EnhancedInputComponent->BindAction(MouseLeftAction, ETriggerEvent::Completed, this, &APlanetPawn::UntriggerSnappedLook);
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &APlanetPawn::StartAim);
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &APlanetPawn::StopAim);
    }
}

void APlanetPawn::GainExperience(float XP)
{
	CurrentXP += XP * ExpGainScale;
    while (CurrentXP >= XPToNextLevel)
    {
        LevelUp();
    }
}

void APlanetPawn::LevelUp()
{
	CurrentXP -= XPToNextLevel;
    CurrentLevel++;
    CalculateXPToNextLevel();

    if (APlanetPlayerController* PC = Cast<APlanetPlayerController>(GetController()))
    {
        PC->ShowRewardSelection();
    }
}

void APlanetPawn::BlockPlayerInput()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(PlayerInputBlock, 1);
            UE_LOG(LogTemp, Warning, TEXT("Player input blocked"));
        }
    }
}

void APlanetPawn::UnblockPlayerInput()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->RemoveMappingContext(PlayerInputBlock);
            UE_LOG(LogTemp, Warning, TEXT("Player input unblocked"));
            bIsSnapLook = bIsAiming = false;
        }
    }
}

void APlanetPawn::HandleDestruction()
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
	MainWeapon->Destroy();

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

AWeaponPawn* APlanetPawn::GetSubWeaponOrNull(EWeaponType WeaponType) const
{ 
    for (AWeaponPawn* EquippedSubWeapon : EquippedSubWeapons)
    {
        if (EquippedSubWeapon->GetWeaponType() == WeaponType)
        {
            return EquippedSubWeapon;
        }
    }
    return nullptr;
}

void APlanetPawn::SetAdditionalHealth(const int32& NewAdditionalHealth)
{
    if (HealthComponent)
    {
        HealthComponent->UpdateHealth(NewAdditionalHealth);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HealthComponent not found on APlanetPawn"));
    }
}

void APlanetPawn::SetDamageScale(const float& NewDamageScale)
{
    DamageScale = NewDamageScale;
    MainWeapon->SetDamageScale(DamageScale);
    for (AWeaponPawn* EquippedSubWeapon : EquippedSubWeapons)
    {
        EquippedSubWeapon->SetDamageScale(DamageScale);
    }
}

void APlanetPawn::SetProjectileSpeedScale(const float& NewPrjectileSpeedScale)
{
    PrjectileSpeedScale = NewPrjectileSpeedScale;
    MainWeapon->SetProjectileSpeedScale(PrjectileSpeedScale);
    for (AWeaponPawn* EquippedSubWeapon : EquippedSubWeapons)
    {
        EquippedSubWeapon->SetProjectileSpeedScale(PrjectileSpeedScale);
    }
}

void APlanetPawn::SetAdditionalCritical(const float& NewAdditionalCritical)
{
    AdditionalCritical = NewAdditionalCritical;
    MainWeapon->SetAdditionalCritical(AdditionalCritical);
    for (AWeaponPawn* EquippedSubWeapon : EquippedSubWeapons)
    {
        EquippedSubWeapon->SetAdditionalCritical(AdditionalCritical);
    }
}

void APlanetPawn::SetHasteScale(const float& NewHasteScale)
{
    HasteScale = NewHasteScale;
    MainWeapon->UpdateFireRate(HasteScale);
    for (AWeaponPawn* EquippedSubWeapon : EquippedSubWeapons)
    {
        EquippedSubWeapon->UpdateFireRate(HasteScale);
    }
}

void APlanetPawn::ComposeBaseSubobject()
{
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	PlanetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Planet Mesh"));
	PlanetMesh->SetupAttachment(CapsuleComp);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(PlanetMesh);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void APlanetPawn::ComposeWeaponSubobject()
{
	if (PlanetMesh == nullptr) return;

	MainWeaponSocket = CreateDefaultSubobject<USceneComponent>(TEXT("Main Weapon Socket"));
	MainWeaponSocket->SetupAttachment(PlanetMesh);

	SubWeaponSocket1 = CreateDefaultSubobject<USceneComponent>(TEXT("Sub Weapon Socket 1"));
	SubWeaponSocket1->SetupAttachment(PlanetMesh);

	SubWeaponSocket2 = CreateDefaultSubobject<USceneComponent>(TEXT("Sub Weapon Socket 2"));
	SubWeaponSocket2->SetupAttachment(PlanetMesh);

	SubWeaponSocket3 = CreateDefaultSubobject<USceneComponent>(TEXT("Sub Weapon Socket 3"));
	SubWeaponSocket3->SetupAttachment(PlanetMesh);

	SubWeaponSocket4 = CreateDefaultSubobject<USceneComponent>(TEXT("Sub Weapon Socket 4"));
	SubWeaponSocket4->SetupAttachment(PlanetMesh);
}

void APlanetPawn::Look(const FInputActionValue& Value)
{
    if (GetController())
    {
        if (bIsSnapLook)
        {
            SnappedLook(Value);
        }
        else
        {
            const FVector2D LookAxisValue = Value.Get<FVector2D>();
            AddControllerYawInput(LookAxisValue.X * GetWorld()->GetDeltaSeconds() * RotationRate);
            AddControllerPitchInput(LookAxisValue.Y * GetWorld()->GetDeltaSeconds() * RotationRate);

            CumulativeRotation += LookAxisValue.Size() * CumulativeCorrection;
            CumulativeRotation = FMath::Fmod(CumulativeRotation, 360.0f);
        }
        BindRotationToAllWeapons();
    }
}

void APlanetPawn::SnappedLook(const FInputActionValue& Value)
{
	// Throttling by SnappedLookCooldown
	if (!CanPerformSnappedLook())
    {
        return;
    }

    const FVector2D LookAxisValue = Value.Get<FVector2D>();

	FRotator CurrentRotation = GetControlRotation();
	SnappedRotation.Pitch = FMath::RoundToFloat(CurrentRotation.Pitch / SnapAngle) * SnapAngle;
	SnappedRotation.Yaw = FMath::RoundToFloat(CurrentRotation.Yaw / SnapAngle) * SnapAngle;

    if (FMath::Abs(LookAxisValue.X) > SnapThreshold)
    {
        SnappedRotation.Yaw += FMath::Sign(LookAxisValue.X) * SnapAngle;
        GetController()->SetControlRotation(SnappedRotation);
        CumulativeRotation += SnapAngle;
        CumulativeRotation = FMath::Fmod(CumulativeRotation, 360.0f);
    }
    if (FMath::Abs(LookAxisValue.Y) > SnapThreshold * 0.9)
    {
        SnappedRotation.Pitch -= FMath::Sign(LookAxisValue.Y) * SnapAngle;
        GetController()->SetControlRotation(SnappedRotation);
        CumulativeRotation += SnapAngle;
        CumulativeRotation = FMath::Fmod(CumulativeRotation, 360.0f);
    }

	LastSnappedLookTime = GetWorld()->GetTimeSeconds();
}

void APlanetPawn::TriggerSnappedLook()
{
	bIsSnapLook = true;
	SpringArm->bEnableCameraRotationLag = true;
}

void APlanetPawn::UntriggerSnappedLook()
{
	bIsSnapLook = false;
	SpringArm->bEnableCameraRotationLag = false;
}

bool APlanetPawn::CanPerformSnappedLook() const
{
	return GetWorld()->GetTimeSeconds() - LastSnappedLookTime >= SnappedLookCooldown;
}

void APlanetPawn::StartAim()
{
	bIsAiming = true;
	RotationRate = DefaultRotationRate / 2;
    CumulativeCorrection = CumulativeDefaultCorrection / 2;
}

void APlanetPawn::StopAim()
{
	bIsAiming = false;
	RotationRate = DefaultRotationRate;
    CumulativeCorrection = CumulativeDefaultCorrection;
}

void APlanetPawn::UpdateArmLength(float DeltaTime)
{
	float TargetArmLength = bIsAiming ? AimArmLength : DefaultArmLength;
    CurrentArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, ArmLengthInterpSpeed);
    SpringArm->TargetArmLength = CurrentArmLength;
}

void APlanetPawn::UpdateOrbitParameters()
{
    ASTEWKGameModeBase* GameMode = Cast<ASTEWKGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GameMode)
    {
        OrbitRadius = GameMode->OrbitRadius;
        OrbitPeriod = GameMode->OrbitPeriod;
    }
    else
    {
        // 게임 모드를 찾지 못한 경우 기본값 사용
        OrbitRadius = 20000.0f;
        OrbitPeriod = 1200.0f;
    }
}

void APlanetPawn::RunOrbit(const float& DeltaTime)
{
	if (CapsuleComp == nullptr) return;

	// 각도 계산
    CurrentAngle += (DeltaTime / OrbitPeriod) * 360.0f;
    CurrentAngle = FMath::Fmod(CurrentAngle, 360.0f);

    // 새로운 위치 계산
    float X = OrbitRadius * FMath::Cos(FMath::DegreesToRadians(CurrentAngle));
    float Y = OrbitRadius * FMath::Sin(FMath::DegreesToRadians(CurrentAngle));

    CapsuleComp->SetRelativeLocation(FVector(X, Y, 0.0f));
}

void APlanetPawn::InitializeWeaponSockets()
{
	SubWeaponSockets = { SubWeaponSocket1, SubWeaponSocket2, SubWeaponSocket3, SubWeaponSocket4 };
}

void APlanetPawn::SpawnMainWeapon()
{
	if (MainWeaponSocket && MainWeaponClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        FVector Location = MainWeaponSocket->GetComponentLocation();
        FRotator Rotation = MainWeaponSocket->GetComponentRotation();

        MainWeapon = GetWorld()->SpawnActor<AWeaponPawn>(MainWeaponClass, Location, Rotation, SpawnParams);
        if (MainWeapon)
        {
            MainWeapon->SetOwner(this);
            MainWeapon->AttachToComponent(MainWeaponSocket, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            MainWeapon->Initialize();
        }
    }
}

void APlanetPawn::AddSubWeapon(EWeaponType WeaponType)
{
    for (AWeaponPawn* EquippedSubWeapon : EquippedSubWeapons)
    {
        if (EquippedSubWeapon->GetWeaponType() == WeaponType)
        {
            UE_LOG(LogTemp, Warning, TEXT("Already equipped sub weapon type: %s"), *UEnum::GetValueAsString(WeaponType));
            return;
        }
    }
    if (EquippedSubWeapons.Num() >= MaxSubWeapons)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add more sub weapons. Maximum limit reached."));
        return;
    }

    TSubclassOf<AWeaponPawn>* WeaponClassPtr = SubWeaponClasses.Find(WeaponType);
    if (!WeaponClassPtr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Weapon class not found for type: %s"), *UEnum::GetValueAsString(WeaponType));
        return;
    }

    int32 Index = EquippedSubWeapons.Num();
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector Location = SubWeaponSockets[Index]->GetComponentLocation();
    FRotator Rotation = SubWeaponSockets[Index]->GetComponentRotation();

    AWeaponPawn* NewWeapon = GetWorld()->SpawnActor<AWeaponPawn>(*WeaponClassPtr, Location, Rotation, SpawnParams);
    if (NewWeapon)
    {
        NewWeapon->SetOwner(this);
        NewWeapon->AttachToComponent(SubWeaponSockets[Index], FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        NewWeapon->Initialize();
        EquippedSubWeapons.Add(NewWeapon);
    }
}

void APlanetPawn::RemoveSubWeapon(int32 Index)
{
	if (EquippedSubWeapons.IsValidIndex(Index))
    {
        EquippedSubWeapons[Index]->Destroy();
        EquippedSubWeapons.RemoveAt(Index);
    }
}

void APlanetPawn::BindRotationToAllWeapons()
{
    if (MainWeapon)
    {
        MainWeapon->HandleMechanicByRotation(CumulativeRotation);
    }
    for (AWeaponPawn* EquippedSubWeapon : EquippedSubWeapons)
    {
        EquippedSubWeapon->HandleMechanicByRotation(CumulativeRotation);
    }
}

void APlanetPawn::CalculateXPToNextLevel()
{
	if (CurrentLevel == 1)
    {
        XPToNextLevel = 5;
    }
    else if (CurrentLevel <= 20)
    {
        XPToNextLevel = 5 + (CurrentLevel - 1) * 10;
    }
    else if (CurrentLevel <= 40)
    {
        XPToNextLevel = 195 + (CurrentLevel - 20) * 13;
    }
    else
    {
        XPToNextLevel = 455 + (CurrentLevel - 40) * 16;
    }

    // Add additional XP requirements for levels 20 and 40
    if (CurrentLevel == 20)
    {
        XPToNextLevel += 600;
    }
    else if (CurrentLevel == 40)
    {
        XPToNextLevel += 2400;
    }
}