// GameTimeManager.cpp
#include "GameTimeManager.h"

UGameTimeManager::UGameTimeManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    ElapsedTime = 0.0f;
    bIsPaused = false;
}

void UGameTimeManager::BeginPlay()
{
    Super::BeginPlay();
    ResetTime();
}

void UGameTimeManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsPaused)
    {
        ElapsedTime += DeltaTime;
    }
}

void UGameTimeManager::PauseTime()
{
    bIsPaused = true;
}

void UGameTimeManager::ResumeTime()
{
    bIsPaused = false;
}

void UGameTimeManager::ResetTime()
{
    ElapsedTime = 0.0f;
    bIsPaused = false;
}