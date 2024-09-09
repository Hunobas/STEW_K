// ZacoBomb.h
#pragma once

#include "CoreMinimal.h"

#include "../EnemyCharacter.h"

#include "ZacoBomb.generated.h"

class UNiagaraSystem;
class USoundBase;
class APlanetPawn;

UCLASS()
class STEW_K_API AZacoBomb : public AEnemyCharacter
{
    GENERATED_BODY()

public:
    AZacoBomb();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void OnPawnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
    void ExplodeAndDamagePlayer(APlanetPawn* PlayerPawn);

	// ====================== 수치 기획 =============================
    UPROPERTY(EditAnywhere, Category = "적")
    float Damage = 10.0f;

	// ====================== 이펙트 =============================
    UPROPERTY(EditAnywhere, Category = "Effects")
    UNiagaraSystem* ExplodeTemplate;
    UPROPERTY(EditAnywhere, Category = "Effects")
    USoundBase* ExplodeSound;

};