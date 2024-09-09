// ZacoBomb.cpp
#include "Components/PrimitiveComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "../../Planet/PlanetPawn.h"

#include "ZacoBomb.h"

AZacoBomb::AZacoBomb()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AZacoBomb::BeginPlay()
{
    Super::BeginPlay();

    Damage *= GetDamageScale();

    if (UPrimitiveComponent* PawnRootComponent = Cast<UPrimitiveComponent>(GetRootComponent()))
    {
        PawnRootComponent->OnComponentHit.AddDynamic(this, &AZacoBomb::OnPawnCollision);
    }
}

void AZacoBomb::OnPawnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (APlanetPawn* PlayerPawn = Cast<APlanetPawn>(OtherActor))
    {
        ExplodeAndDamagePlayer(PlayerPawn);
    }
}

void AZacoBomb::ExplodeAndDamagePlayer(APlanetPawn* PlayerPawn)
{
    if (PlayerPawn)
    {
        FDamageEvent DamageEvent;
        PlayerPawn->TakeDamage(Damage, DamageEvent, GetController(), this);
    }

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        ExplodeTemplate,
        GetActorLocation(),
        GetActorRotation()
    );
    UGameplayStatics::PlaySoundAtLocation(
        GetWorld(),
        ExplodeSound,
        GetActorLocation()
    );
    Destroy();
}