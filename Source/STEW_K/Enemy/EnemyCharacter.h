// EnemyCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "EnemyCharacter.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class USceneComponent;
class UHealthComponent;
class APlanetPawn;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class STEW_K_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ReadyToShoot();
	virtual void Shoot();
	void HandleDestruction();

	// ====================== 게터 =============================
	UFUNCTION(BlueprintCallable, Category = "Health")
    float GetHealthPercentage() const;
	
	FVector GetAimPointLocation() const { return AimPoint->GetComponentLocation(); }
	float GetHealthScale() const { return HealthScale; }
	float GetDamageScale() const { return DamageScale; }
	float GetSpeedScale() const { return SpeedScale; }
	float GetXPDropScale() const { return XPDropScale; }

	// ====================== 세터 =============================
	void SetHealthScale(const float& NewHealthScale) { HealthScale = NewHealthScale; }
	void SetDamageScale(const float& NewDamageScale) { DamageScale = NewDamageScale; }
	void SetSpeedScale(const float& NewSpeedScale) { SpeedScale = NewSpeedScale; }

protected:
    APlanetPawn* PlayerPawn;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UCapsuleComponent* CapsuleComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* EnemyMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* AimPoint;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UHealthComponent* HealthComponent;

	// ====================== 수치 기획 =============================
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float JustAimDelay = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float JustAimDamage = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float ShootDelay = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float HealthScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float DamageScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float SpeedScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float XPDropScale = 1.f;

    FTimerHandle ShootTimerHandle;

	// ====================== 이펙트 =============================
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    UNiagaraSystem* JustAimTemplate;
	UPROPERTY(EditAnywhere, Category = "이펙트")
    UNiagaraSystem* ShotMuzzleTemplate;
    UPROPERTY(EditAnywhere, Category = "이펙트")
    USoundBase* ShotSound;
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    UNiagaraSystem* DestructionTemplate;
    UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    USoundBase* DestructionSound;
	
};
