// ZacoBeam.cpp
#include "Components/SceneComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "../../Planet/PlanetPawn.h"

#include "ZacoBeam.h"


AZacoBeam::AZacoBeam()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AZacoBeam::BeginPlay()
{
    Super::BeginPlay();

    Damage *= GetDamageScale();
}

void AZacoBeam::Shoot()
{
    Super::Shoot();

    if (APlanetPawn* PlayerPawn = Cast<APlanetPawn>(UGameplayStatics::GetPlayerPawn(this, 0)))
    {
        FDamageEvent DamageEvent;
        PlayerPawn->TakeDamage(Damage, DamageEvent, GetController(), this);
    }
}