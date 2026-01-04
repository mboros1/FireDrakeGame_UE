#include "DwarfCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "HealthComponent.h"
#include "AIController.h"
#include "BrainComponent.h"

ADwarfCharacter::ADwarfCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 350.0f;
	GetCharacterMovement()->JumpZVelocity = 420.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	MeleeRangeComponent = CreateDefaultSubobject<USphereComponent>(TEXT("MeleeRangeComponent"));
	MeleeRangeComponent->SetupAttachment(RootComponent);
	MeleeRangeComponent->SetSphereRadius(MeleeRange);
	MeleeRangeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ADwarfCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnHealthDepleted.AddDynamic(this, &ADwarfCharacter::OnDeath);
	}
}

void ADwarfCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAnimationState();
}

void ADwarfCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADwarfCharacter, AnimState);
	DOREPLIFETIME(ADwarfCharacter, bIsOnFire);
	DOREPLIFETIME(ADwarfCharacter, FlailIntensity);
}

void ADwarfCharacter::SetAnimState(EDwarfAnimState NewState)
{
	if (AnimState != NewState)
	{
		AnimState = NewState;
	}
}

void ADwarfCharacter::PerformMeleeAttack()
{
	// TODO: impl
}

void ADwarfCharacter::SetOnFire(float Duration)
{
	// TODO: impl
}

void ADwarfCharacter::StopBeingOnFire()
{
	bIsOnFire = false;
	if (AnimState == EDwarfAnimState::OnFire)
	{
		SetAnimState(EDwarfAnimState::Idle);
	}
}


void ADwarfCharacter::UpdateAnimationState()
{

	float Speed = GetVelocity().Size();
	if (Speed > 10.0f)
	{
		if (AnimState != EDwarfAnimState::Run)
		{
			SetAnimState(EDwarfAnimState::Run);
		}
	}
	else
	{
		if (AnimState != EDwarfAnimState::Idle)
		{
			SetAnimState(EDwarfAnimState::Idle);
		}
	}
}

void ADwarfCharacter::OnDeath()
{
	// TODO: impl
}
