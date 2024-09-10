// ZacoBeam.h
#pragma once

#include "CoreMinimal.h"
#include "../EnemyCharacter.h"
#include "ZacoBeam.generated.h"

class USceneComponent;
class UNiagaraSystem;
class USoundBase;

/**
 * 
 */
UCLASS()
class STEW_K_API AZacoBeam : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	AZacoBeam();

	virtual void Shoot() override;

protected:
    virtual void BeginPlay() override;
	
private:
	// ====================== 수치 기획 =============================
    UPROPERTY(EditAnywhere, Category = "적")
    float Damage = 8.0f;

};
