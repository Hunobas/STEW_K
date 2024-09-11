// CelestialBody.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "CelestialBody.generated.h"

class UCapsuleComponent;
class USceneComponent;
class APlanetPawn;

UCLASS()
class STEW_K_API ACelestialBody : public AActor
{
    GENERATED_BODY()
    
public: 
    ACelestialBody();
    virtual void Tick(float DeltaTime) override;

    static ACelestialBody* GetInstance();
    USceneComponent* GetRandomActiveAimPointOrNull();
    void MarkAimPointAsOccupied(USceneComponent* AimPoint);
    void MarkAimPointAsFree(USceneComponent* AimPoint);

protected:
    virtual void BeginPlay() override;

private:
    static ACelestialBody* Instance;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UCapsuleComponent* CapsuleComp;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    TArray<USceneComponent*> AimPoints;

    UPROPERTY()
    APlanetPawn* PlayerPawn;

    TArray<bool> AimPointActivation;
    TArray<bool> AimPointOccupation;

    void UpdateAimPointActivation();
	bool IsInFrustum(const FVector& Location, float Radius = 0.0f);

};
