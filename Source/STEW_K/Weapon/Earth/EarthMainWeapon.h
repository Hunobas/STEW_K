// EarthMainWeapon.h
#pragma once

#include "CoreMinimal.h"

#include "../WeaponPawn.h"

#include "EarthMainWeapon.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UNiagaraSystem;
class USoundBase;
class AMainweaponProjectile;

/**
 * 
 */
UCLASS()
class STEW_K_API AEarthMainWeapon : public AWeaponPawn
{
	GENERATED_BODY()

public:
	AEarthMainWeapon();

	virtual void Tick(float DeltaTime) override;

    virtual void WeaponLevelUp(const int32& NewCurrentWeaponLevel) override;

	// ====================== 레벨 업 =============================
	UPROPERTY(EditDefaultsOnly, Category = "레벨 업")
	float DamageUp_LV2 = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "레벨 업")
	float ProjectileSpeedUp_LV4 = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "레벨 업")
	float DamageUp_LV6 = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "레벨 업")
	float LaserDamage_LV7 = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "레벨 업")
	float LaserRate_LV7 = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "레벨 업")
	float MaxRange_LV7 = 8000.f;
	UPROPERTY(EditDefaultsOnly, Category = "레벨 업")
	int32 MaxHitInOneCycle_LV7 = 2;

	virtual void SetFireRate(const float& NewFireRate) override
	{
		Super::SetFireRate(NewFireRate);
		LaserTraceDuration = GetFireRate() * 0.8f;
	}
	virtual void UpdateFireRate(const float& HasteScale) override
	{
		Super::UpdateFireRate(HasteScale);
		LaserTraceDuration = GetFireRate() * 0.8f;
	}

protected:
	virtual void FireProjectile() override;

private:
	AMainweaponProjectile* SpawnProjectileAtPointOrNull(USceneComponent* SpawnPoint);

	void FirePositronRifle();
	void PerformLaserTrace();
    void ApplyLaserDamage(AActor* Target);
    void ClearLaserTrace();

	bool bReleaseSideSpawnPoint = false;
	bool bReleaseUpdownSpawnPoint = false;
	bool bReleasePositronRifle = false;

	FTimerHandle LaserTraceTimerHandle;
    FTimerHandle LaserCooldownTimerHandle;
	TArray<AActor*> TracedActors;
    TMap<AActor*, float> ActorDamageTimestamps;

	float FireTime;
	float LaserTraceDuration = 3.2f;
    const float LaserTraceInterval = 0.08f;

	// ====================== 기본 구성 =============================
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* ProjectileSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* LeftSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* RightSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* UpperSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* LowerSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
    TSubclassOf<AMainweaponProjectile> ProjectileClass;

	// ====================== 이펙트 =============================
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    USoundBase* ShotSound;
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    UNiagaraSystem* LaserTemplate;
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    UNiagaraSystem* LaserHitTemplate;
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    UNiagaraSystem* LaserExplodeTemplate;
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    USoundBase* LaserSound;
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    USoundBase* LaserExplodeSound;
	
};
