#include "DwarfAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UDwarfAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	DwarfCharacter = Cast<ADwarfCharacter>(TryGetPawnOwner());
}

void UDwarfAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!DwarfCharacter || !DwarfCharacter->IsValidLowLevel())
	{
		return;
	}

	AnimationState = DwarfCharacter->AnimState;
	bIsOnFire = DwarfCharacter->bIsOnFire;
	FlailIntensity = DwarfCharacter->FlailIntensity;

	FVector Velocity = DwarfCharacter->GetVelocity();
	Speed = Velocity.Size();

	bIsInAir = DwarfCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = DwarfCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;

	if (DwarfCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
	{
		FRotator RotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(
			DwarfCharacter->GetCharacterMovement()->GetCurrentAcceleration().Rotation(),
			DwarfCharacter->GetActorRotation()
		);
		Direction = RotationDifference.Yaw;
	}
}