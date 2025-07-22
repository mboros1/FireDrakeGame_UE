#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WyvernCharacter.h"
#include "WyvernAnimInstance.generated.h"

UCLASS(Blueprintable)
class FIREDRAKE_API UWyvernAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/** C++ updates this every frame; the State Machine just reads it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	EWyvernMovementState MovementState = EWyvernMovementState::Idle;
};