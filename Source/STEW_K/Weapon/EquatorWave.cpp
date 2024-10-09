// EquatorWave.cpp
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "../Planet/PlanetPawn.h"

#include "EquatorWave.h"

AEquatorWave::AEquatorWave()
{
    PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;

    WaveCenter = CreateDefaultSubobject<USceneComponent>(TEXT("Wave Center"));
	WaveCenter->SetupAttachment(WeaponMesh);

    EastSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("East Spawn Point"));
	EastSpawnPoint->SetupAttachment(WaveCenter);

    WestSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("West Spawn Point"));
	WestSpawnPoint->SetupAttachment(WaveCenter);

    NorthSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("North Spawn Point"));
	NorthSpawnPoint->SetupAttachment(WaveCenter);

    SouthSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("South Spawn Point"));
	SouthSpawnPoint->SetupAttachment(WaveCenter);

}

void AEquatorWave::Initialize()
{
	Super::Initialize();

    if (OwnerPlanet)
    {
        SetActorLocation(OwnerPlanet->GetActorLocation());
        SetActorRotation(OwnerPlanet->GetActorRotation());
    }
    WeaponLevelUp(1);
}

void AEquatorWave::WeaponLevelUp(const int32& NewCurrentWeaponLevel)
{
    Super::WeaponLevelUp(NewCurrentWeaponLevel);

    switch (CurrentWeaponLevel)
    {
        case 1:
            bReleaseEastSpawnPoint = true;
            Fire();
            break;
        case 2:
            bReleaseWestSpawnPoint = true;
            Fire();
            break;
        case 3:
            SetDamageScale(GetDamageScale() + DamageUp_LV3);
            break;
        case 4:
            bReleaseNorthSpawnPoint = true;
            Fire();
            break;
        case 5:
            UpdateFireRate(GetFireRate() + FireRateUp_LV5);
            break;
        case 6:
            SetDamageScale(GetDamageScale() + DamageUp_LV6);
            break;
        case 7:
            bReleaseSouthSpawnPoint = true;
            WaveCenter->SetWorldRotation(FRotator(0.0f, 45.0f, 7.0f));
            Fire();
            break;
        default:
            break;
    }
}

void AEquatorWave::HandleMechanicByRotation(const float& Rotation)
{
    Super::HandleMechanicByRotation(Rotation);

    if (!OwnerPlanet)
    {
        return;
    }

    float WaveValueDegree = FMath::Sin(FMath::DegreesToRadians(2*Rotation)) * Amplitude;
    float WaveValue = FMath::DegreesToRadians(WaveValueDegree);

    if (bReleaseEastSpawnPoint)
    {
        UpdateSpawnPointLocation(EastSpawnPoint, WaveValue, false);
    }
    if (bReleaseWestSpawnPoint)
    {
        UpdateSpawnPointLocation(WestSpawnPoint, -WaveValue, false);
    }
    if (bReleaseNorthSpawnPoint)
    {
        UpdateSpawnPointLocation(NorthSpawnPoint, WaveValue, true);
    }
    if (bReleaseSouthSpawnPoint)
    {
        UpdateSpawnPointLocation(SouthSpawnPoint, -WaveValue, true);
    }
}

void AEquatorWave::UpdateSpawnPointLocation(USceneComponent* SpawnPoint, float WaveValue, bool bIsMeridian)
{
    FVector EarthCenter = GetActorLocation();
    FRotator EarthRotation = GetActorRotation();

    // 적도 평면상의 위치 계산
    FVector EquatorOffset;
    if (!bIsMeridian)
    {
        EquatorOffset = 100.f * FVector(FMath::Cos(WaveValue), FMath::Sin(WaveValue), 0.f);
    }
    else
    {
        EquatorOffset = 100.f * FVector(FMath::Cos(WaveValue), 0.f, FMath::Sin(WaveValue));
    }
    
    // 지구 자전에 따른 회전 적용
    FVector NewSpawnLocation = EarthCenter + EarthRotation.RotateVector(EquatorOffset);

    FVector SpawnPointDirection = (NewSpawnLocation - EarthCenter).GetSafeNormal();
    FRotator NewSpawnRotation = SpawnPointDirection.Rotation();

    SpawnPoint->SetWorldLocation(NewSpawnLocation);
    SpawnPoint->SetWorldRotation(NewSpawnRotation);
}

void AEquatorWave::SetDamageScale(const float& NewDamageScale)
{
    Super::SetDamageScale(NewDamageScale);

    Damage *= GetDamageScale();
}

void AEquatorWave::SetAdditionalCritical(const float& NewAdditionalCritical)
{
    Super::SetAdditionalCritical(NewAdditionalCritical);

    Critical += GetAdditionalCritical();
}

void AEquatorWave::UpdateFireRate(const float& HasteScale)
{
    LaserHitRate = DefaultLaserHitRate / HasteScale;

    Super::UpdateFireRate(HasteScale);
}

void AEquatorWave::Fire()
{
    Super::Fire();

    GetWorldTimerManager().ClearTimer(FireTimerHandle);

    if (bReleaseEastSpawnPoint)
    {
        FireFromSpawnPoint(EastLaserTraceTimerHandle, EastSpawnPoint);
    }
    if (bReleaseWestSpawnPoint)
    {
        FireFromSpawnPoint(WestLaserTraceTimerHandle, WestSpawnPoint);
    }
    if (bReleaseNorthSpawnPoint)
    {
        FireFromSpawnPoint(NorthLaserTraceTimerHandle, NorthSpawnPoint);
    }
    if (bReleaseSouthSpawnPoint)
    {
        FireFromSpawnPoint(SouthLaserTraceTimerHandle, SouthSpawnPoint);
    }
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), LaserSound, GetActorLocation());
}

void AEquatorWave::FireFromSpawnPoint(FTimerHandle& LaserTraceTimerHandle, USceneComponent* SpawnPoint)
{
    UNiagaraFunctionLibrary::SpawnSystemAttached(
        LaserTemplate,
        SpawnPoint,
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true
    );

    GetWorldTimerManager().SetTimer(LaserTraceTimerHandle, FTimerDelegate::CreateUObject(this, &AEquatorWave::PerformLaserTrace, SpawnPoint), LaserHitRate, true);
}

void AEquatorWave::PerformLaserTrace(USceneComponent* SpawnPoint)
{
    FVector TraceStart = SpawnPoint->GetComponentLocation();
    FVector TraceEnd = TraceStart + SpawnPoint->GetForwardVector() * MaxRange;
    
    float SphereRadius = 45.0f;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    FHitResult HitResult;
    bool bHit = GetWorld()->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, FQuat::Identity, ECC_GameTraceChannel2, SphereShape, QueryParams);

    if (bHit)
    {
        ApplyLaserDamage(HitResult);
    }
}

void AEquatorWave::ApplyLaserDamage(const FHitResult& HitResult)
{
    float FinalDamage = Damage;
    bool bIsCriticalHit = FMath::RandRange(0.0f, 1.0f) < Critical;
    if (bIsCriticalHit)
    {
        FinalDamage *= 2.0f;
    }

    AActor* HitActor = HitResult.GetActor();
    if (HitActor)
    {
        UGameplayStatics::ApplyDamage(
            HitActor,
            FinalDamage,
            GetInstigatorController(),
            this,
            UDamageType::StaticClass()
        );
    }

    UNiagaraSystem* EffectToSpawn = bIsCriticalHit ? CriticalLaserHitTemplate : LaserHitTemplate;
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        EffectToSpawn,
        HitResult.ImpactPoint,
        HitResult.ImpactNormal.Rotation()
    );
}
