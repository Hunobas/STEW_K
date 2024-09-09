// XPGem.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "XPGem.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class USoundBase;
class APlanetPawn;

UCLASS()
class STEW_K_API AXPGem : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AXPGem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// ====================== 게터 =============================
	float GetSpeedScale() const { return SpeedScale; }

private:
	void PlayerGainXP();

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCapsuleComponent* CapsuleComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* GemMesh;

	APlanetPawn* PlayerPawn;

	// ====================== 수치 기획 =============================
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float XP = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float SpeedScale = 1.5f;

	// ====================== 이펙트 =============================
    UPROPERTY(EditAnywhere, Category = "Effects")
    UNiagaraSystem* GainTemplate;
    UPROPERTY(EditAnywhere, Category = "Effects")
    USoundBase* GainSound;

};
