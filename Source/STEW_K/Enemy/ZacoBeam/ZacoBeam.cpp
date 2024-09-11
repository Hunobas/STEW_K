// ZacoBeam.cpp
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"

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

    PlayerPawn = Cast<APlanetPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
    Damage *= GetDamageScale();

    GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AEnemyCharacter::ReadyToShoot, FireRate, true);
    ReadyToShoot();
}

void AZacoBeam::Shoot()
{
    Super::Shoot();

    if (!PlayerPawn)
    {
        return;
    }

    const FHitResult* HitResult = GetPlayerAimHitResult();
    if (HitResult)
    {
        PlayerPawn->SucceedJustAim(*HitResult);
        TakeDamage(40.0f, FDamageEvent(), GetController(), this);
    }
    else
    {
        FDamageEvent DamageEvent;
        PlayerPawn->TakeDamage(Damage, DamageEvent, GetController(), this);
    }
}

const FHitResult* AZacoBeam::GetPlayerAimHitResult() const
{
    if (!PlayerPawn)
    {
        return nullptr;
    }

    FCollisionShape SweepShape = FCollisionShape::MakeSphere(JustAimAcceptableRadius);

    FVector SweepStart = PlayerPawn->GetSweepStartLocation();
    FVector SweepEnd = PlayerPawn->GetSweepEndLocation();

    DrawDebugLine(GetWorld(), SweepStart, SweepEnd, FColor::Red, false);

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