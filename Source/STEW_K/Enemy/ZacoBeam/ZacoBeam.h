// ZacoBeam.h
#pragma once

#include "CoreMinimal.h"
#include "../EnemyCharacter.h"
#include "ZacoBeam.generated.h"

class USceneComponent;
class UNiagaraSystem;
class USoundBase;
class APlanetPawn;

/**
 * 
 */
UCLASS()
class STEW_K_API AZacoBeam : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	AZacoBeam();

	virtual void Tick(float DeltaTime) override;
	virtual void Shoot() override;
    virtual void HandleDestruction() override;

protected:
    virtual void BeginPlay() override;
	
private:
    FTimerHandle FireTimerHandle;

	// ====================== 수치 기획 =============================
    UPROPERTY(EditDefaultsOnly, Category = "적")
    float Damage = 8.f;
    UPROPERTY(EditDefaultsOnly, Category = "적")
    float FireRate = 5.f;

};
