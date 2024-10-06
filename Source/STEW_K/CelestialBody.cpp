// CelestialBody.cpp
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Planet/PlanetPawn.h"

#include "CelestialBody.h"


ACelestialBody::ACelestialBody()
{
    PrimaryActorTick.bCanEverTick = true;

    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
    RootComponent = CapsuleComp;

    const float YawInterval = 360.0f / PointsPerRow;

    int32 PointIndex = 0;
    for (int32 Row = 0; Row < NumRows; ++Row)
    {
        float RowPitch = (Row - (NumRows - 1) / 2.0f) * RowPitchInterval;

        for (int32 PointInRow = 0; PointInRow < PointsPerRow; ++PointInRow)
        {
            float Yaw = PointInRow * YawInterval;

            FString AimPointName = FString::Printf(TEXT("Aim Point %02d"), PointIndex + 1);
            USceneComponent* AimPoint = CreateDefaultSubobject<USceneComponent>(*AimPointName);
            AimPoint->SetupAttachment(CapsuleComp);

            // 구면 좌표계를 직교 좌표계로 변환
            FVector Position;
            Position.X = SphereRadius * FMath::Cos(FMath::DegreesToRadians(RowPitch)) * FMath::Cos(FMath::DegreesToRadians(Yaw));
            Position.Y = SphereRadius * FMath::Cos(FMath::DegreesToRadians(RowPitch)) * FMath::Sin(FMath::DegreesToRadians(Yaw));
            Position.Z = SphereRadius * FMath::Sin(FMath::DegreesToRadians(RowPitch));

            AimPoint->SetRelativeLocation(Position);
            
            // AimPoint가 구체의 중심을 향하도록 회전
            FRotator Rotation = Position.Rotation();
            Rotation.Yaw += 180.0f; // AimPoint의 전방 벡터가 구체의 중심을 향하도록 180도 회전
            AimPoint->SetRelativeRotation(Rotation);

            AimPoints.Add(AimPoint);
            PointIndex++;
        }
    }
}

void ACelestialBody::BeginPlay()
{
    Super::BeginPlay();
    PlayerPawn = Cast<APlanetPawn>(UGameplayStatics::GetPlayerPawn(this, 0));

    AimPointActivation.Init(true, AimPoints.Num());
    AimPointOccupation.Init(false, AimPoints.Num());
}

void ACelestialBody::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    SetActorLocation(PlayerPawn->GetActorLocation());
}

USceneComponent* ACelestialBody::GetRandomActiveAimPointOrNull() const
{
    // UpdateAimPointActivation();

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

TArray<USceneComponent*> ACelestialBody::GetNthPointsRowOrNull(const int32& n) const
{
    TArray<USceneComponent*> ResultRow;
    if (n < 1 || n > NumRows)
    {
        return ResultRow;
    }

    int32 StartIndex = (n - 1) * PointsPerRow;
    int32 EndIndex = n * PointsPerRow;

    for (int32 i = StartIndex; i < EndIndex && i < AimPoints.Num(); ++i)
    {
        ResultRow.Add(AimPoints[i]);
    }

    return ResultRow;
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

void ACelestialBody::FreeAllAimPoints()
{
    for (bool& Occupation : AimPointOccupation)
    {
        Occupation = false;
    }
}

// void ACelestialBody::UpdateAimPointActivation()
// {
//     for (int32 i = 0; i < AimPoints.Num(); i++)
//     {
//         FVector AimPointLocation = AimPoints[i]->GetComponentLocation();
//         AimPointActivation[i] = IsInFrustum(AimPointLocation);
//     }
// }

// bool ACelestialBody::IsInFrustum(const FVector& Location, float Radius)
// {
//     APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
//     if (!PlayerController)
//     {
//         return false;
//     }

//     ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
//     if (!LocalPlayer || !LocalPlayer->ViewportClient || !LocalPlayer->ViewportClient->Viewport)
//     {
//         return false;
//     }

//     FSceneViewProjectionData ProjectionData;
//     if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, ProjectionData))
//     {
//         FConvexVolume Frustum;
//         GetViewFrustumBounds(Frustum, ProjectionData.ComputeViewProjectionMatrix(), true);

//         return Frustum.IntersectSphere(Location, Radius);
//     }

//     return false;
// }