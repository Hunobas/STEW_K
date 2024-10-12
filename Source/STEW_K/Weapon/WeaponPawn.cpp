// WeaponPawn.cpp
#include "Kismet/GameplayStatics.h"

#include "../Player/PlanetPawn.h"

#include "WeaponPawn.h"


// Called when the game starts or when spawned
void AWeaponPawn::Initialize()
{
    OwnerPlanet = Cast<APlanetPawn>(GetOwner());
    if (OwnerPlanet)
    {
        DamageScale *= OwnerPlanet->GetDamageScale();
        SpeedScale *= OwnerPlanet->GetProjectileSpeedScale();
        AdditionalCritical += OwnerPlanet->GetAdditionalCritical();
        UpdateFireRate(OwnerPlanet->GetHasteScale());
    }
}

void AWeaponPawn::WeaponLevelUp(const int32& NewCurrentWeaponLevel)
{
    CurrentWeaponLevel = NewCurrentWeaponLevel;
}

void AWeaponPawn::HandleMechanicByRotation(const float& Rotation)
{
    // Nothing to do here.
}

void AWeaponPawn::Fire()
{
    // Nothing to do here.
}
