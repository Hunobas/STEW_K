// Sun.cpp
#include "Components/StaticMeshComponent.h"

#include "Sun.h"

// Sets default values
ASun::ASun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sun Mesh"));
	RootComponent = SunMesh;

}

// Called when the game starts or when spawned
void ASun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

