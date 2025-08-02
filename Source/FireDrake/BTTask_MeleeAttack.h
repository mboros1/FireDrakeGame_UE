#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MeleeAttack.generated.h"

UCLASS()
class FIREDRAKE_API UBTTask_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MeleeAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDuration = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float DamageDelay = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	FBlackboardKeySelector TargetActorKey;

private:
	float AttackTimer = 0.0f;
	bool bHasDealtDamage = false;

	void DealDamage(UBehaviorTreeComponent& OwnerComp);
};