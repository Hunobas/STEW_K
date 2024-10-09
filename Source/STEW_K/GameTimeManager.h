// GameTimeManager.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GameTimeManager.generated.h"

/*
 *
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class STEW_K_API UGameTimeManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UGameTimeManager();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Game Time")
    float GetElapsedTime() const { return ElapsedTime; }

    UFUNCTION(BlueprintCallable, Category = "Game Time")
    void PauseTime();
    UFUNCTION(BlueprintCallable, Category = "Game Time")
    void ResumeTime();
    UFUNCTION(BlueprintCallable, Category = "Game Time")
    void ResetTime();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    float ElapsedTime;

    bool bIsPaused;
};