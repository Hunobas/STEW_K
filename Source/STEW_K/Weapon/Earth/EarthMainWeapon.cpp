// EarthMainWeapon.cpp
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "MainweaponProjectile.h"

#include "EarthMainWeapon.h"


AEarthMainWeapon::AEarthMainWeapon()
{
    PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(WeaponMesh);

    SpawnPointDrum = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point Drum"));
	SpawnPointDrum->SetupAttachment(WeaponMesh);

    LeftSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Left Spawn Point"));
	LeftSpawnPoint->SetupAttachment(SpawnPointDrum);

    RightSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Right Spawn Point"));
	RightSpawnPoint->SetupAttachment(SpawnPointDrum);

    UpperSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Upper Spawn Point"));
	UpperSpawnPoint->SetupAttachment(SpawnPointDrum);

    LowerSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Lower Spawn Point"));
	LowerSpawnPoint->SetupAttachment(SpawnPointDrum);

}

void AEarthMainWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AEarthMainWeapon::WeaponLevelUp(const int32& NewCurrentWeaponLevel)
{
    Super::WeaponLevelUp(NewCurrentWeaponLevel);

    switch (CurrentWeaponLevel)
    {
        case 2:
            SetDamageScale(GetDamageScale() + DamageUp_LV2);
            SetAdditionalPenetration(GetAdditionalPenetration() + 1);
            break;
        case 3:
            bReleaseSideSpawnPoint = true;
            break;
        case 4:
            SetProjectileSpeedScale(GetProjectileSpeedScale() * ProjectileSpeedUp_LV4);
            break;
        case 5:
            bReleaseUpdownSpawnPoint = true;
            break;
        case 6:
            SetDamageScale(GetDamageScale() + DamageUp_LV6);
            SetAdditionalPenetration(GetAdditionalPenetration() + 1);
            break;
        case 7:
            bReleasePositronRifle = true;
            SetFireRate(LaserRate_LV7);
            break;
        default:
            break;
    }
}

void AEarthMainWeapon::FireProjectile()
{
    Super::FireProjectile();

    if (bReleasePositronRifle)
    {
        return FirePositronRifle();
    }

    AMainweaponProjectile* MainProjectile = SpawnProjectileAtPointOrNull(ProjectileSpawnPoint);
    if (MainProjectile)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            ShotSound,
            ProjectileSpawnPoint->GetComponentLocation()
        );
    }

    if (bReleaseSideSpawnPoint)
    {
        SpawnProjectileAtPointOrNull(LeftSpawnPoint);
        SpawnProjectileAtPointOrNull(RightSpawnPoint);
    }
    if (bReleaseUpdownSpawnPoint)
    {
        SpawnProjectileAtPointOrNull(UpperSpawnPoint);
        SpawnProjectileAtPointOrNull(LowerSpawnPoint);
    }
}

AMainweaponProjectile* AEarthMainWeapon::SpawnProjectileAtPointOrNull(USceneComponent* SpawnPoint)
{
    if (ProjectileClass && SpawnPoint)
    {
        FVector SpawnLocation = SpawnPoint->GetComponentLocation();
        FRotator SpawnRotation = SpawnPoint->GetComponentRotation();

        AMainweaponProjectile* Projectile = GetWorld()->SpawnActor<AMainweaponProjectile>(
            ProjectileClass,
            SpawnLocation,
            SpawnRotation
        );
        if (Projectile)
        {
            Projectile->SetOwner(this);
            Projectile->Initialize();
            return Projectile;
        }
    }
    return nullptr;
}

void AEarthMainWeapon::FirePositronRifle()
{
    UNiagaraFunctionLibrary::SpawnSystemAttached(
        LaserTemplate,
        ProjectileSpawnPoint,
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true
    );
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), LaserSound, GetActorLocation());

    TracedActors.Empty();
    ActorDamageTimestamps.Empty();
    FireTime = GetWorld()->GetTimeSeconds();
    GetWorldTimerManager().SetTimer(LaserTraceTimerHandle, this, &AEarthMainWeapon::PerformLaserTrace, LaserTraceInterval, true);
    GetWorldTimerManager().SetTimer(LaserCooldownTimerHandle, this, &AEarthMainWeapon::ClearLaserTrace, LaserTraceDuration, false);
}

void AEarthMainWeapon::PerformLaserTrace()
{
    FVector TraceStart = ProjectileSpawnPoint->GetComponentLocation();
    FVector TraceEnd = TraceStart + ProjectileSpawnPoint->GetForwardVector() * MaxRange_LV7;
    
    float SphereRadius = 45.0f;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    TArray<FHitResult> HitResults;
    GetWorld()->SweepMultiByChannel(HitResults, TraceStart, TraceEnd, FQuat::Identity, ECC_GameTraceChannel2, SphereShape, QueryParams);

    int32 ValidHits = 0;
    for (const FHitResult& Hit : HitResults)
    {
        if (ValidHits >= 1 + GetAdditionalPenetration()) break;

        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            LaserHitTemplate,
            Hit.ImpactPoint,
            Hit.ImpactNormal.Rotation()
        );

        AActor* HitActor = Hit.GetActor();
        if (FireTime && HitActor && !TracedActors.Contains(HitActor))
        {
            TracedActors.Add(HitActor);
            ValidHits++;

            UWidgetComponent* HealthBarWidget = Cast<UWidgetComponent>(HitActor->GetComponentByClass(UWidgetComponent::StaticClass()));
            if (HealthBarWidget && HealthBarWidget->GetName() == "Health Bar Widget")
            {
                HealthBarWidget->SetHiddenInGame(false);
            }

            float CurrentTime = GetWorld()->GetTimeSeconds();
            // 아직 안 맞았거나 맞은 뒤 LaserTraceDuration / MaxHitInOneCycle_LV7 초가 흐르면 데미지 적용
            if (!ActorDamageTimestamps.Contains(HitActor) || CurrentTime - ActorDamageTimestamps[HitActor] >= (LaserTraceDuration / MaxHitInOneCycle_LV7))
            {
                // 드라마틱한 연출을 위해 한번에 터지는 딜레이 시간 계산
                float LaserDamageDelay = (CurrentTime - FireTime) * (FireTime - CurrentTime) / LaserTraceDuration + LaserTraceDuration;
                ActorDamageTimestamps.Add(HitActor, CurrentTime);
                FTimerHandle DamageTimerHandle;
                // 데미지 적용 예약
                GetWorldTimerManager().SetTimer(DamageTimerHandle, FTimerDelegate::CreateUObject(this, &AEarthMainWeapon::ApplyLaserDamage, HitActor), LaserDamageDelay, false);
                // ApplyLaserDamage(HitActor);
            }
        }
    }
}

void AEarthMainWeapon::ApplyLaserDamage(AActor* Target)
{
    if (Target)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            LaserExplodeTemplate,
            Target->GetActorLocation(),
            FRotator::ZeroRotator
        );
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            LaserExplodeSound,
            Target->GetActorLocation()
        );

        UGameplayStatics::ApplyDamage(
            Target,
            LaserDamage_LV7,
            GetInstigatorController(),
            this,
            UDamageType::StaticClass()
        );
    }
}

void AEarthMainWeapon::ClearLaserTrace()
{
    GetWorldTimerManager().ClearTimer(LaserTraceTimerHandle);
    TracedActors.Empty();
    ActorDamageTimestamps.Empty();
}