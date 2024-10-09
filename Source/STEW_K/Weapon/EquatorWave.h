// EquatorWave.h
#pragma once

#include "CoreMinimal.h"
#include "WeaponPawn.h"

#include "EquatorWave.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UNiagaraSystem;
class USoundBase;
class APlanetPawn;

/**
 * 
 */
UCLASS()
class STEW_K_API AEquatorWave : public AWeaponPawn
{
	GENERATED_BODY()

public:
	AEquatorWave();

	virtual void Initialize() override;
    virtual void WeaponLevelUp(const int32& NewCurrentWeaponLevel) override;
	virtual void HandleMechanicByRotation(const float& Rotation) override;

	// ====================== 세터 =============================
	virtual void SetDamageScale(const float& NewDamageScale) override;
	virtual void SetAdditionalCritical(const float& NewAdditionalCritical) override;
	virtual void UpdateFireRate(const float& HasteScale) override;

	// ====================== 레벨 업 =============================
	UPROPERTY(EditDefaultsOnly, Category = "레벨 업")
	float DamageUp_LV3 = 0.25f;
	UPROPERTY(EditDefaultsOnly, Category = "레벨 업")
	float FireRateUp_LV5 = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "레벨 업")
	float DamageUp_LV6 = 0.5f;

protected:
	virtual void Fire() override;

private:
	void FireFromSpawnPoint(FTimerHandle& LaserTraceTimerHandle, USceneComponent* SpawnPoint);
	void PerformLaserTrace(USceneComponent* SpawnPoint);
    void ApplyLaserDamage(const FHitResult& HitResult);

	void UpdateSpawnPointLocation(USceneComponent* SpawnPoint, float WaveValue, bool bIsMeridian);

	APlanetPawn* PlayerPawn;

	FTimerHandle EastLaserTraceTimerHandle;
	FTimerHandle WestLaserTraceTimerHandle;
	FTimerHandle NorthLaserTraceTimerHandle;
	FTimerHandle SouthLaserTraceTimerHandle;

	bool bReleaseEastSpawnPoint = false;
	bool bReleaseWestSpawnPoint = false;
	bool bReleaseNorthSpawnPoint = false;
	bool bReleaseSouthSpawnPoint = false;

	// ====================== 기본 구성 =============================
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* WaveCenter;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* EastSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* WestSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* NorthSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* SouthSpawnPoint;

	// ====================== 수치 기획 =============================
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float Damage = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float Critical = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float MaxRange = 8000.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float Amplitude = 15.0f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float DefaultLaserHitRate = 0.08f;
	UPROPERTY()
    float LaserHitRate = 0.08f;

	// ====================== 이펙트 =============================
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    UNiagaraSystem* LaserTemplate;
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    UNiagaraSystem* LaserHitTemplate;
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    UNiagaraSystem* CriticalLaserHitTemplate;
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    USoundBase* LaserSound;
	
};
