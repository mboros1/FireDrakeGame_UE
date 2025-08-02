#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
	DOREPLIFETIME(UHealthComponent, MaxHealth);
}

void UHealthComponent::TakeDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f || !IsAlive())
	{
		return;
	}

	float NewHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	UpdateHealth(NewHealth);
}

void UHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || !IsAlive())
	{
		return;
	}

	float NewHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	UpdateHealth(NewHealth);
}

void UHealthComponent::UpdateHealth(float NewHealth)
{
	if (FMath::IsNearlyEqual(CurrentHealth, NewHealth))
	{
		return;
	}

	CurrentHealth = NewHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		OnHealthDepleted.Broadcast();
	}

	OnRep_CurrentHealth();
}

void UHealthComponent::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	
	if (CurrentHealth <= 0.0f)
	{
		OnHealthDepleted.Broadcast();
	}
}