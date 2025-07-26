#include "WyvernCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "WyvernAnimInstance.h"
#include "Components/PointLightComponent.h"

AWyvernCharacter::AWyvernCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 650.0f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create spring arm component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 800.0f;
	SpringArmComponent->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	// Create camera component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	// Create fire glow light
	FireGlowLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FireGlowLight"));
	FireGlowLight->SetupAttachment(RootComponent);
	FireGlowLight->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f)); // Position above dragon
	FireGlowLight->SetIntensity(5.0f);
	FireGlowLight->SetLightColor(FLinearColor(1.0f, 0.4f, 0.0f)); // Deeper orange color
	FireGlowLight->SetAttenuationRadius(1200.0f);
	FireGlowLight->SetSourceRadius(200.0f); // Makes light act like a larger sphere
	FireGlowLight->SetSoftSourceRadius(100.0f); // Softens shadows
	FireGlowLight->bUseInverseSquaredFalloff = false;
	FireGlowLight->SetCastShadows(false); // Disable shadows to prevent self-shadowing
	FireGlowLight->SetVolumetricScatteringIntensity(0.5f);

	// Initialize movement state
	CurrentMovementState = EWyvernMovementState::Idle;
	bIsFlying = false;
	bIsTakingOff = false;
	bIsAscending = false;
	bIsDescending = false;
	bIsLanding = false;
	TakeOffTimer = 0.0f;
}

void AWyvernCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AWyvernCharacter::StartJump()
{
    if (!bIsFlying && !bIsTakingOff)
    {
        // mark that we’re in the “take-off” state
        bIsTakingOff        = true;
        TakeOffTimer        = 0.f;
        CurrentMovementState = EWyvernMovementState::TakingOff;

        /* ❶ This calls ACharacter::Jump()
               → applies JumpZVelocity as an impulse
               → sets MovementMode to MOVE_Falling
               → toggles bPressedJump so the engine keeps the
                 capsule airborne for the standard jump-hold time */
        Jump();
    }
    else if (bIsFlying)
    {
        bIsAscending         = true;
        CurrentMovementState = EWyvernMovementState::Flapping;
    }
}

void AWyvernCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovementState();

	if (bIsFlying)
	{
		HandleFlyingMovement(DeltaTime);
	}
	else
	{
		HandleGroundMovement(DeltaTime);
	}

	// Handle takeoff
	if (bIsTakingOff)
	{
	    if (GetVelocity().Z <= 0.f)
	    {
		bIsTakingOff = false;
		bIsFlying    = true;
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	    }
	}

	// Handle landing when descending near ground
	if (bIsFlying && bIsDescending)
	{
		// Check distance to ground
		FHitResult Hit;
		FVector Start = GetActorLocation();
		FVector End = Start - FVector(0, 0, LandingDistance + GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
		{
			// We're close to the ground, initiate landing
			bIsFlying = false;
			bIsDescending = false;
			bIsLanding = false;  // Landing is instant for now
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			CurrentMovementState = EWyvernMovementState::Idle;
		}
	}
}

void AWyvernCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AWyvernCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWyvernCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AWyvernCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AWyvernCharacter::StopJump);
	PlayerInputComponent->BindAction("Descend", IE_Pressed, this, &AWyvernCharacter::StartDescend);
	PlayerInputComponent->BindAction("Descend", IE_Released, this, &AWyvernCharacter::StopDescend);
}

void AWyvernCharacter::MoveForward(float Value)
{
	MovementInput.X = Value;

	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		if (bIsFlying)
		{
			AddMovementInput(Direction, Value);
		}
		else
		{
			AddMovementInput(Direction, Value);
		}
	}
}

void AWyvernCharacter::MoveRight(float Value)
{
	MovementInput.Y = Value;

	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(Direction, Value);
	}
}

void AWyvernCharacter::StopJump()
{
	if (bIsFlying)
	{
		bIsAscending = false;
		if (MovementInput.Size() > 0.1f)
		{
			CurrentMovementState = EWyvernMovementState::Flying;
		}
		else
		{
			CurrentMovementState = EWyvernMovementState::Flapping;
		}
	}
}

void AWyvernCharacter::StartDescend()
{
	if (bIsFlying && !bIsLanding)
	{
		bIsDescending = true;
		CurrentMovementState = EWyvernMovementState::Flying;
	}
}

void AWyvernCharacter::StopDescend()
{
	if (bIsFlying)
	{
		bIsDescending = false;
		if (MovementInput.Size() > 0.1f)
		{
			CurrentMovementState = EWyvernMovementState::Flying;
		}
		else
		{
			CurrentMovementState = EWyvernMovementState::Flapping;
		}
	}
}

void AWyvernCharacter::UpdateMovementState()
{
	if (bIsTakingOff)
	{
		CurrentMovementState = EWyvernMovementState::TakingOff;
		SyncAnimState();
		return;
	}

	if (bIsFlying)
	{
		if (bIsAscending)
		{
			CurrentMovementState = EWyvernMovementState::Flapping;
		}
		else if (GetVelocity().Size() > 10.0f)
		{
			CurrentMovementState = EWyvernMovementState::Flying;
		}
		else
		{
			CurrentMovementState = EWyvernMovementState::Flapping;
		}
	}
	else
	{
		if (GetVelocity().Size() > 10.0f)
		{
			CurrentMovementState = EWyvernMovementState::Walking;
		}
		else
		{
			CurrentMovementState = EWyvernMovementState::Idle;
		}
	}
	
	SyncAnimState();
}

void AWyvernCharacter::HandleGroundMovement(float DeltaTime)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AWyvernCharacter::HandleFlyingMovement(float DeltaTime)
{
	GetCharacterMovement()->MaxFlySpeed = FlySpeed;
	
	if (bIsAscending)
	{
		FVector UpwardForce = FVector(0, 0, AscendSpeed);
		GetCharacterMovement()->AddInputVector(UpwardForce * DeltaTime);
	}
	else if (bIsDescending)
	{
		FVector DownwardForce = FVector(0, 0, -DescendSpeed);
		GetCharacterMovement()->AddInputVector(DownwardForce * DeltaTime);
	}
}

void AWyvernCharacter::SyncAnimState()
{
	if (USkeletalMeshComponent* Mesh = GetMesh())
	{
		if (UWyvernAnimInstance* Anim = Cast<UWyvernAnimInstance>(Mesh->GetAnimInstance()))
		{
			Anim->MovementState = CurrentMovementState;
		}
	}
}
