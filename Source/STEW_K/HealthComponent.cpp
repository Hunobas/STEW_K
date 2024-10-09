// HealthComponent.cpp
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"

#include "STEWKGameModeBase.h"
#include "Planet/PlanetPawn.h"
#include "Enemy/EnemyCharacter.h"

#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	STEWKGameModeBase = Cast<ASTEWKGameModeBase>(UGameplayStatics::GetGameMode(this));

	if(APlanetPawn* PlanetOwner = Cast<APlanetPawn>(GetOwner()))
	{
		MaxHealth += PlanetOwner->GetAdditionalHealth();
		PlanetOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
	}
	else if(AEnemyCharacter* EnemyOwner = Cast<AEnemyCharacter>(GetOwner()))
	{
		MaxHealth *= EnemyOwner->GetHealthScale();
		EnemyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
	}
	
    MaxHealth = FMath::Max(MaxHealth, 1.0f);
    Health = MaxHealth;
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UHealthComponent::UpdateHealth(const int32& NewAdditionalHealth)
{
	if (MaxHealth > 0)
	{
		float HealthPercent = Health / MaxHealth;
		MaxHealth += NewAdditionalHealth;
		Health = MaxHealth * HealthPercent;
	}
}

void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	if (Damage <= 0.f) return;

	Health -= Damage;

	if(STEWKGameModeBase && Health <= 0)
	{
		STEWKGameModeBase->ActorDied(DamagedActor, true);
	}
}

