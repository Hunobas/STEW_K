// WeaponPawn.cpp
#include "Kismet/GameplayStatics.h"

#include "../Planet/PlanetPawn.h"

#include "WeaponPawn.h"

// Called when the game starts or when spawned
void AWeaponPawn::BeginPlay()
{
	Super::BeginPlay();

    if (APlanetPawn* OwnerPlanet = Cast<APlanetPawn>(GetOwner()))
    {
        DamageScale *= OwnerPlanet->GetDamageScale();
        AdditionalPenetration += OwnerPlanet->GetAdditionalPenetration();
        SpeedScale *= OwnerPlanet->GetProjectileSpeedScale();
        AdditionalCritical += OwnerPlanet->GetAdditionalCritical();
        UpdateFireRate(OwnerPlanet->GetHasteScale());
    }
}

void AWeaponPawn::WeaponLevelUp(const int32& NewCurrentWeaponLevel)
{
    CurrentWeaponLevel = NewCurrentWeaponLevel;
}

void AWeaponPawn::FireProjectile()
{
    // Nothing to do here.
}


