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
	if (!bCanAttack || AnimState == EDwarfAnimState::Dead)
	{
		return;
	}

	bCanAttack = false;
	SetAnimState(EDwarfAnimState::Attack);

	GetWorld()->GetTimerManager().SetTimer(AttackCooldownHandle, this, &ADwarfCharacter::ResetAttackCooldown, AttackCooldown, false);
}

void ADwarfCharacter::SetOnFire(float Duration)
{
	if (AnimState == EDwarfAnimState::Dead)
	{
		return;
	}

	bIsOnFire = true;
	SetAnimState(EDwarfAnimState::OnFire);

	GetWorld()->GetTimerManager().ClearTimer(OnFireTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(OnFireTimerHandle, this, &ADwarfCharacter::StopBeingOnFire, Duration, false);
}

void ADwarfCharacter::StopBeingOnFire()
{
	bIsOnFire = false;
	if (AnimState == EDwarfAnimState::OnFire)
	{
		SetAnimState(EDwarfAnimState::Idle);
	}
}

void ADwarfCharacter::ResetAttackCooldown()
{
	bCanAttack = true;
	if (AnimState == EDwarfAnimState::Attack)
	{
		SetAnimState(EDwarfAnimState::Idle);
	}
}

float ADwarfCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent)
	{
		HealthComponent->TakeDamage(ActualDamage);

		if (HealthComponent->GetCurrentHealth() > 0 && AnimState != EDwarfAnimState::OnFire)
		{
			SetAnimState(EDwarfAnimState::HitReact);
			
			FTimerHandle HitReactTimer;
			GetWorld()->GetTimerManager().SetTimer(HitReactTimer, [this]()
			{
				if (AnimState == EDwarfAnimState::HitReact)
				{
					SetAnimState(EDwarfAnimState::Idle);
				}
			}, 0.5f, false);
		}
	}

	return ActualDamage;
}

void ADwarfCharacter::UpdateAnimationState()
{
	if (AnimState == EDwarfAnimState::Dead || AnimState == EDwarfAnimState::OnFire || AnimState == EDwarfAnimState::Attack)
	{
		return;
	}

	float Speed = GetVelocity().Size();
	if (Speed > 10.0f)
	{
		if (AnimState != EDwarfAnimState::Jog)
		{
			SetAnimState(EDwarfAnimState::Jog);
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
	SetAnimState(EDwarfAnimState::Dead);

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->GetBrainComponent()->StopLogic("Dead");
	}

	SetLifeSpan(10.0f);
}