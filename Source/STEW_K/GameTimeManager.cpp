// GameTimeManager.cpp
#include "GameTimeManager.h"

UGameTimeManager* UGameTimeManager::Instance = nullptr;

UGameTimeManager::UGameTimeManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    ElapsedTime = 0.0f;
    bIsPaused = false;
}

void UGameTimeManager::BeginPlay()
{
    Super::BeginPlay();
    
    if (!Instance)
    {
        Instance = this;
    }
}

void UGameTimeManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsPaused)
    {
        ElapsedTime += DeltaTime;
    }
}

UGameTimeManager* UGameTimeManager::GetInstance()
{
    if (!Instance)
    {
        Instance = NewObject<UGameTimeManager>();
        Instance->AddToRoot(); // 가비지 컬렉션 방지
    }
    return Instance;
}

void UGameTimeManager::PauseTime()
{
    bIsPaused = true;
}

void UGameTimeManager::ResumeTime()
{
    bIsPaused = false;
}