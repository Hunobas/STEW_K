// PlanetPawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"

#include "PlanetPawn.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class USceneComponent;
class UHealthComponent;
class UInputMappingContext;
class UInputAction;
class UNiagaraSystem;
class USoundBase;

class AWeaponPawn;

UENUM(BlueprintType)
enum class EPlanetType : uint8
{
    Earth
};

UCLASS()
class STEW_K_API APlanetPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlanetPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GainExperience(float XP);
    void LevelUp();
	void HandleDestruction();

	// ====================== 게터 =============================
	EPlanetType GetPlanetType() const { return PlanetType; }
	int32 GetAdditionalHealth() const { return AdditionalHealth; }
	float GetDamageScale() const { return DamageScale; }
	float GetProjectileSpeedScale() const { return PrjectileSpeedScale; }
	float GetAdditionalCritical() const { return AdditionalCritical; }
	float GetHasteScale() const { return HasteScale; }
	int32 GetAdditionalPenetration() const { return AdditionalPenetration; }
	float GetExpGainScale() const { return ExpGainScale; }
	float GetExpSpeedScale() const { return ExpSpeedScale; }

	UFUNCTION(BlueprintPure)
	int32 GetCurrentLevel() const { return CurrentLevel; }
	UFUNCTION(BlueprintPure)
    float GetCurrentXP() const { return CurrentXP; }
	UFUNCTION(BlueprintPure)
    int32 GetXPToNextLevel() const { return XPToNextLevel; }

    int32 GetMaxSubWeapons() const { return MaxSubWeapons; }
    int32 GetMaxPassiveItems() const { return MaxPassiveItems; }
	int32 GetWeaponSlotsFilled() const { return WeaponSlotsFilled; }
	int32 GetPassiveItemSlotsFilled() const { return PassiveItemSlotsFilled; }

	AWeaponPawn* GetMainWeapon() const { return MainWeapon; }

	// ====================== 세터 =============================
	void SetAdditionalHealth(const int32& NewAdditionalHealth);
	void SetDamageScale(const float& NewDamageScale);
	void SetProjectileSpeedScale(const float& NewPrjectileSpeedScale);
	void SetAdditionalCritical(const float& NewAdditionalCritical);
	void SetHasteScale(const float& NewHasteScale);
	void SetAdditionalPenetration(const int32& NewAdditionalPenetration);
	void SetExpGainScale(const float& NewExpGainScale) { ExpGainScale = NewExpGainScale; }
	void SetExpSpeedScale(const float& NewExpSpeedScale) { ExpSpeedScale = NewExpSpeedScale; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* PlayerMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MouseLeftAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float DefaultRotationRate = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float RotationRate;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float SnapAngle = 15.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float SnapThreshold = 0.8f;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "0.0"))
    float SnappedLookCooldown = 0.05f;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "0.0"))
    float DefaultArmLength = 640.0f;
    UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "0.0"))
    float AimArmLength = 240.0f;
    UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "0.0"))
    float ArmLengthInterpSpeed = 5.0f;

private:
	void ComposeBaseSubobject();
	void ComposeWeaponSubobject();
	void Look(const FInputActionValue& Value);
	void SnappedLook(const FInputActionValue& Value);
	void TriggerSnappedLook();
	void UntriggerSnappedLook();
	bool CanPerformSnappedLook() const;

	void StartAim();
    void StopAim();
	void UpdateArmLength(float DeltaTime);

	void UpdateOrbitParameters();
	void RunOrbit(const float& DeltaTime);

	void InitializeWeaponSockets();
    void SpawnMainWeapon();
    void AddSubWeapon(TSubclassOf<AWeaponPawn> WeaponClass);
    void RemoveSubWeapon(int32 Index);

	void CalculateXPToNextLevel();

	UPROPERTY(EditDefaultsOnly, Category = "Components")
    EPlanetType PlanetType;

	// ====================== 기본 구성 =============================
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCapsuleComponent* CapsuleComp;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* PlanetMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USpringArmComponent* SpringArm;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
    UHealthComponent* HealthComponent;

	// ====================== 인게임 무기 =============================
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* MainWeaponSocket;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* SubWeaponSocket1;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* SubWeaponSocket2;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* SubWeaponSocket3;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USceneComponent* SubWeaponSocket4;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
    TArray<USceneComponent*> SubWeaponSockets;

	UPROPERTY(EditDefaultsOnly, Category = "무기 슬롯")
	TSubclassOf<AWeaponPawn> MainWeaponClass;
	UPROPERTY()
    AWeaponPawn* MainWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "무기 슬롯")
	int32 WeaponSlotsFilled = 4;
	UPROPERTY(EditDefaultsOnly, Category = "무기 슬롯")
	int32 PassiveItemSlotsFilled = 6;
	UPROPERTY(VisibleDefaultsOnly, Category = "무기 슬롯")
	int32 MaxSubWeapons = 4;
	UPROPERTY(VisibleDefaultsOnly, Category = "무기 슬롯")
	int32 MaxPassiveItems = 6;
	UPROPERTY(VisibleDefaultsOnly, Category = "무기 슬롯")
	int32 TypesOfSubWeapons = 6;
	UPROPERTY(EditDefaultsOnly, Category = "무기 슬롯")
    TArray<TSubclassOf<AWeaponPawn>> SubWeaponClasses;
	UPROPERTY()
    TArray<AWeaponPawn*> EquippedSubWeapons;

	// ====================== 레벨링 =============================
	UPROPERTY(EditDefaultsOnly, Category = "레벨링")
    int32 CurrentLevel;
    UPROPERTY(EditDefaultsOnly, Category = "레벨링")
    float CurrentXP;
    UPROPERTY(EditDefaultsOnly, Category = "레벨링")
    int32 XPToNextLevel;

	// ============================================================

	// 카메라 스냅 룩, 카메라 에임
	float LastSnappedLookTime;
	bool bIsSnapLookTriggered;
    FRotator SnappedRotation;
	float CurrentArmLength;
	bool bIsAiming;

	// 행성 공전 메커니즘
	float OrbitRadius;
    float OrbitPeriod;
    float CurrentAngle;
	
	// ====================== 수치 기획 =============================
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	int32 AdditionalHealth = 0;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float DamageScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	int32 AdditionalPenetration = 0;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float PrjectileSpeedScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float AdditionalCritical = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float HasteScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float ExpGainScale = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "전투 스케일")
	float ExpSpeedScale = 1.5f;

	// ====================== 이펙트 =============================
	UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    UNiagaraSystem* DestructionTemplate;
    UPROPERTY(EditDefaultsOnly, Category = "이펙트")
    USoundBase* DestructionSound;

};
