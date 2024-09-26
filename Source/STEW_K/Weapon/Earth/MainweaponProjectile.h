// MainweaponProjectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MainweaponProjectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS()
class STEW_K_API AMainweaponProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMainweaponProjectile();

public:	
	void Initialize();

private:
	UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// ====================== 기본 구성 =============================
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UBoxComponent* CollisionBox;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* LeftBulletMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* RightBulletMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
    float LifeSpan = 4.0f;

	// ====================== 수치 기획 =============================
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float Damage = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float Speed = 5000.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
    float Critical = 0.1f;

	// ====================== 이펙트 =============================
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    UNiagaraSystem* HitTemplate;
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
	UNiagaraSystem* CriticalHitTemplate;

};
