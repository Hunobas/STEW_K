// Sun.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Sun.generated.h"

UCLASS()
class STEW_K_API ASun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Static Mesh")
	UStaticMeshComponent* SunMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
