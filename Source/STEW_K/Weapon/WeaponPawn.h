// WeaponPawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "../Reward/WeaponReward.h"

#include "WeaponPawn.generated.h"

class APlanetPawn;

UCLASS()
class STEW_K_API AWeaponPawn : public APawn
{
	GENERATED_BODY()

public:	
	virtual void Initialize();
    virtual void WeaponLevelUp(const int32& NewCurrentWeaponLevel);
	virtual void HandleMechanicByRotation(const float& Rotation);

	// ====================== 게터 =============================
	EWeaponType GetWeaponType() const { return WeaponType; }
	int32 GetCurrentWeaponLevel() const { return CurrentWeaponLevel; }
	float GetDamageScale() const { return DamageScale; }
	float GetProjectileSpeedScale() const { return SpeedScale; }
	float GetAdditionalCritical() const { return AdditionalCritical; }
	float GetFireRate() const { return FireRate; }

	// ====================== 세터 =============================
	virtual void SetDamageScale(const float& NewDamageScale) { DamageScale = NewDamageScale; }
	virtual void SetProjectileSpeedScale(const float& NewSpeedScale) { SpeedScale = NewSpeedScale; }
	virtual void SetAdditionalCritical(const float& NewAdditionalCritical) { AdditionalCritical = NewAdditionalCritical; }
	virtual void SetFireRate(const float& NewFireRate)
	{
		DefaultFireRate = NewFireRate;
		FireRate = NewFireRate;
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponPawn::Fire, FireRate, true);
		Fire();
	}
	virtual void UpdateFireRate(const float& HasteScale)
	{
		FireRate = DefaultFireRate / HasteScale;
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponPawn::Fire, FireRate, true);
		Fire();
	}

protected:
	virtual void Fire();

	UPROPERTY(EditDefaultsOnly, Category = "Components")
    EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "디버깅")
	int32 CurrentWeaponLevel = 0;

	APlanetPawn* OwnerPlanet;
	FTimerHandle FireTimerHandle;

private:
	// ====================== 수치 기획 =============================
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float DamageScale = 1.0f;
    UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float SpeedScale = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float AdditionalCritical = 0.0f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float DefaultFireRate = 0.2f;
	UPROPERTY()
    float FireRate = 0.2f;

};
