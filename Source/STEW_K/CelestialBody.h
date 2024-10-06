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

    USceneComponent* GetRandomActiveAimPointOrNull() const;
    TArray<USceneComponent*> GetNthPointsRowOrNull(const int32& n) const;
    void MarkAimPointAsOccupied(USceneComponent* AimPoint);
    void MarkAimPointAsFree(USceneComponent* AimPoint);
    void FreeAllAimPoints();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Components")
    UCapsuleComponent* CapsuleComp;

    UPROPERTY(EditDefaultsOnly, Category = "Components")
    TArray<USceneComponent*> AimPoints;

    UPROPERTY(EditDefaultsOnly, Category = "스폰 포인트")
    float SphereRadius = 1500.f;
    UPROPERTY(EditDefaultsOnly, Category = "스폰 포인트")
    int32 PointsPerRow = 24;
    UPROPERTY(EditDefaultsOnly, Category = "스폰 포인트")
    int32 NumRows = 5;
    UPROPERTY(EditDefaultsOnly, Category = "스폰 포인트")
    float RowPitchInterval = 15.0f;

    UPROPERTY()
    APlanetPawn* PlayerPawn;

    TArray<bool> AimPointActivation;
    TArray<bool> AimPointOccupation;

    // void UpdateAimPointActivation();
	// bool IsInFrustum(const FVector& Location, float Radius = 0.0f);

};
