// WeaponPawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "../Reward/WeaponReward.h"

#include "WeaponPawn.generated.h"

UCLASS()
class STEW_K_API AWeaponPawn : public APawn
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
    virtual void WeaponLevelUp(const int32& NewCurrentWeaponLevel);

	// ====================== 게터 =============================
	int32 GetCurrentWeaponLevel() const { return CurrentWeaponLevel; }
	float GetDamageScale() const { return DamageScale; }
	int32 GetAdditionalPenetration() const { return AdditionalPenetration; }
	float GetProjectileSpeedScale() const { return SpeedScale; }
	float GetAdditionalCritical() const { return AdditionalCritical; }
	float GetFireRate() const { return FireRate; }

	// ====================== 세터 =============================
	void SetDamageScale(const float& NewDamageScale) { DamageScale = NewDamageScale; }
	void SetAdditionalPenetration(const int32& NewAdditionalPenetration) { AdditionalPenetration = NewAdditionalPenetration; }
	void SetProjectileSpeedScale(const float& NewSpeedScale) { SpeedScale = NewSpeedScale; }
	void SetAdditionalCritical(const float& NewAdditionalCritical) { AdditionalCritical = NewAdditionalCritical; }
	void SetFireRate(const float& NewFireRate)
	{
		FireRate = NewFireRate;
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponPawn::FireProjectile, FireRate, true);
	}
	void UpdateFireRate(const float& HasteScale)
	{
		FireRate /= HasteScale;
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AWeaponPawn::FireProjectile, FireRate, true);
	}

protected:
	virtual void FireProjectile();

	UPROPERTY(EditDefaultsOnly, Category = "Components")
    EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "디버깅")
	int32 CurrentWeaponLevel = 0;

private:
	// ====================== 수치 기획 =============================
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float DamageScale = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    int32 AdditionalPenetration = 0;
    UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float SpeedScale = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float AdditionalCritical = 0.0f;
    UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float FireRate = 0.2f;

	FTimerHandle FireTimerHandle;

};
