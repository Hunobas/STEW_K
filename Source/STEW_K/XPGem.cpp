// XPGem.cpp
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "Planet/PlanetPawn.h"

#include "XPGem.h"

// Sets default values
AXPGem::AXPGem()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::Spawned;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	GemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gem Mesh"));
	GemMesh->SetupAttachment(CapsuleComp);
	
}

// Called when the game starts or when spawned
void AXPGem::BeginPlay()
{
    Super::BeginPlay();

    if (CapsuleComp)
    {
        CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AXPGem::OnOverlapBegin);
    }

    PlayerPawn = Cast<APlanetPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (PlayerPawn)
    {
        SpeedScale = PlayerPawn->GetExpSpeedScale();
    }
}

void AXPGem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (PlayerPawn)
    {
        PlayerGainXP();
    }
}

void AXPGem::PlayerGainXP()
{
	if (PlayerPawn)
	{
		PlayerPawn->GainExperience(XP);
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        GainTemplate,
        GetActorLocation(),
        GetActorRotation()
    );
    UGameplayStatics::PlaySoundAtLocation(
        GetWorld(),
        GainSound,
        GetActorLocation()
    );
    Destroy();
}

