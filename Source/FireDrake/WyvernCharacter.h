#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WyvernCharacter.generated.h"

UENUM(BlueprintType)
enum class EWyvernMovementState : uint8
{
	Idle,
	Walking,
	TakingOff,
	Flying,
	Flapping
};

UCLASS()
class FIREDRAKE_API AWyvernCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWyvernCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	class UPointLightComponent* FireGlowLight;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EWyvernMovementState CurrentMovementState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FlySpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AscendSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DescendSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TakeOffDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LandingDistance = 10.0f;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void StartJump();
	void StopJump();
	void StartDescend();
	void StopDescend();
	
	void UpdateMovementState();
	void HandleGroundMovement(float DeltaTime);
	void HandleFlyingMovement(float DeltaTime);
	void SyncAnimState();
	
	bool bIsFlying;
	bool bIsTakingOff;
	bool bIsAscending;
	bool bIsDescending;
	bool bIsLanding;
	float TakeOffTimer;
	
	FVector MovementInput;
	FVector LastVelocity;
};
