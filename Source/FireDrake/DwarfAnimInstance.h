#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "DwarfCharacter.h"
#include "DwarfAnimInstance.generated.h"

UCLASS(Blueprintable)
class FIREDRAKE_API UDwarfAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation State")
	EDwarfAnimState AnimationState = EDwarfAnimState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation State")
	bool bIsOnFire = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation State")
	float FlailIntensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsAccelerating = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Direction = 0.0f;

private:
	UPROPERTY()
	ADwarfCharacter* DwarfCharacter = nullptr;
};