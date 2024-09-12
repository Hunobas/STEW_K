// ZacoBeam.cpp
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/DamageEvents.h"

#include "../../Planet/PlanetPawn.h"

#include "ZacoBeam.h"


AZacoBeam::AZacoBeam()
{
    PrimaryActorTick.bCanEverTick = true;

}

void AZacoBeam::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerPawn)
    {
        SetActorRotation((PlayerPawn->GetActorLocation() - GetActorLocation()).Rotation());
    }
}

void AZacoBeam::BeginPlay()
{
    Super::BeginPlay();

    Damage *= GetDamageScale();

    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyCharacter::ReadyToShoot, FireRate, true);
}

void AZacoBeam::Shoot()
{
    Super::Shoot();

    if (PlayerPawn)
    {
        FDamageEvent DamageEvent;
        PlayerPawn->TakeDamage(Damage, DamageEvent, GetController(), this);
    }
}