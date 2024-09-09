// HealthComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "HealthComponent.generated.h"

class AController;
class ASTEWKGameModeBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEW_K_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void UpdateHealth(const int32& NewAdditionalHealth);

	UFUNCTION(BlueprintPure)
	float GetMaxHealth() const { return MaxHealth; }
	UFUNCTION(BlueprintPure)
	float GetHealth() const { return Health; }

private:
	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser);
	
	ASTEWKGameModeBase* STEWKGameModeBase;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.f;
	float Health;

};
