#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "DwarfCharacter.generated.h"

UENUM(BlueprintType)
enum class EDwarfAnimState : uint8
{
	Idle,
	Run,
	OnFire
};

UCLASS()
class FIREDRAKE_API ADwarfCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADwarfCharacter();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation", Replicated)
	EDwarfAnimState AnimState = EDwarfAnimState::Idle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation", Replicated)
	bool bIsOnFire = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation", Replicated)
	float FlailIntensity = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EDwarfMovementState CurrentMovementState;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MeleeRange = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float MeleeDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float AttackCooldown = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float DetectionRange = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float LoseTargetRange = 1500.0f;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformMeleeAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetOnFire(float Duration);

	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetAnimState(EDwarfAnimState NewState);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* MeleeRangeComponent;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FTimerHandle OnFireTimerHandle;
	FTimerHandle AttackCooldownHandle;
	bool bCanAttack = true;

	void StopBeingOnFire();
	void ResetAttackCooldown();
	void UpdateAnimationState();
	void OnDeath();
};
