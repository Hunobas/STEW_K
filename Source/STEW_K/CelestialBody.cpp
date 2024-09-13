// CelestialBody.cpp
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Planet/PlanetPawn.h"

#include "CelestialBody.h"


ACelestialBody* ACelestialBody::Instance = nullptr;

ACelestialBody::ACelestialBody()
{
    PrimaryActorTick.bCanEverTick = true;

    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
    RootComponent = CapsuleComp;

    for (int32 i = 1; i <= AimPointsNum; i++)
    {
        FString AimPointName = FString::Printf(TEXT("Aim Point %02d"), i);
        USceneComponent* AimPoint = CreateDefaultSubobject<USceneComponent>(*AimPointName);
        AimPoint->SetupAttachment(CapsuleComp);
        AimPoints.Add(AimPoint);
    }
}

void ACelestialBody::BeginPlay()
{
    Super::BeginPlay();
    PlayerPawn = Cast<APlanetPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
    Instance = this;

    AimPointActivation.Init(false, AimPoints.Num());
    AimPointOccupation.Init(false, AimPoints.Num());
}

void ACelestialBody::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    SetActorLocation(PlayerPawn->GetActorLocation());
}

ACelestialBody* ACelestialBody::GetInstance()
{
    return Instance;
}

USceneComponent* ACelestialBody::GetRandomActiveAimPointOrNull()
{
    UpdateAimPointActivation();

    TArray<USceneComponent*> ActiveUnoccupiedAimPoints;
    for (int32 i = 0; i < AimPoints.Num(); i++)
    {
        if (AimPointActivation[i] && !AimPointOccupation[i])
        {
            ActiveUnoccupiedAimPoints.Add(AimPoints[i]);
        }
    }

    if (ActiveUnoccupiedAimPoints.Num() > 0)
    {
        return ActiveUnoccupiedAimPoints[FMath::RandRange(0, ActiveUnoccupiedAimPoints.Num() - 1)];
    }
    return nullptr;
}

void ACelestialBody::MarkAimPointAsOccupied(USceneComponent* AimPoint)
{
    int32 Index = AimPoints.Find(AimPoint);
    if (Index != INDEX_NONE)
    {
        AimPointOccupation[Index] = true;
    }
}

void ACelestialBody::MarkAimPointAsFree(USceneComponent* AimPoint)
{
    int32 Index = AimPoints.Find(AimPoint);
    if (Index != INDEX_NONE)
    {
        AimPointOccupation[Index] = false;
    }
}

void ACelestialBody::UpdateAimPointActivation()
{
    for (int32 i = 0; i < AimPoints.Num(); i++)
    {
        FVector AimPointLocation = AimPoints[i]->GetComponentLocation();
        AimPointActivation[i] = IsInFrustum(AimPointLocation);
    }
}

bool ACelestialBody::IsInFrustum(const FVector& Location, float Radius)
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController)
    {
        return false;
    }

    ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
    if (!LocalPlayer || !LocalPlayer->ViewportClient || !LocalPlayer->ViewportClient->Viewport)
    {
        return false;
    }

    FSceneViewProjectionData ProjectionData;
    if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, ProjectionData))
    {
        FConvexVolume Frustum;
        GetViewFrustumBounds(Frustum, ProjectionData.ComputeViewProjectionMatrix(), true);

        return Frustum.IntersectSphere(Location, Radius);
    }

    return false;
}