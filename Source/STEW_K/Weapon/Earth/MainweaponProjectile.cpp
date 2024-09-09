// MainweaponProjectile.cpp
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "../WeaponPawn.h"

#include "MainweaponProjectile.h"

// Sets default values
AMainweaponProjectile::AMainweaponProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileRoot"));

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
    CollisionBox->SetupAttachment(RootComponent);

    LeftBulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Bullet"));
    LeftBulletMesh->SetupAttachment(CollisionBox);

    RightBulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Bullet"));
    RightBulletMesh->SetupAttachment(CollisionBox);

	LeftBulletMesh->SetCollisionProfileName(TEXT("NoCollision"));
    RightBulletMesh->SetCollisionProfileName(TEXT("NoCollision"));

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
    ProjectileMovementComponent->SetUpdatedComponent(GetRootComponent());

	InitialLifeSpan = LifeSpan;
}

// Called every frame
void AMainweaponProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainweaponProjectile::Initialize()
{
    ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	if (AWeaponPawn* OwnerWeapon = Cast<AWeaponPawn>(GetOwner()))
    {
		Damage *= OwnerWeapon->GetDamageScale();
		MaxPenetration += OwnerWeapon->GetAdditionalPenetration();
		Speed *= OwnerWeapon->GetProjectileSpeedScale();
		Critical += OwnerWeapon->GetAdditionalCritical();
		CollisionBox->OnComponentHit.AddDynamic(this, &AMainweaponProjectile::OnHit);
		ProjectileMovementComponent->Velocity = GetActorForwardVector() * Speed;
	}
}

void AMainweaponProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    AActor* MyOwner = GetOwner();
    if (MyOwner && OtherActor && OtherActor != this && OtherActor != MyOwner)
    {
        if (!HitActors.Contains(OtherActor))
        {
            HitActors.Add(OtherActor);

            // 크리티컬 히트 계산
            float FinalDamage = Damage;
            bool bIsCriticalHit = FMath::RandRange(0.0f, 1.0f) < Critical;
            if (bIsCriticalHit)
            {
                FinalDamage *= 2.0f;  // 크리티컬 시 2배 데미지
            }

            UGameplayStatics::ApplyDamage(
                OtherActor,
                FinalDamage,
                MyOwner->GetInstigatorController(),
                this,
                UDamageType::StaticClass()
            );

            UNiagaraSystem* EffectToSpawn = bIsCriticalHit ? CriticalHitTemplate : HitTemplate;
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                EffectToSpawn,
                GetActorLocation(),
                GetActorRotation()
            );

            if (--MaxPenetration == 0)
            {
                Destroy();
            }
        }
    }
}

