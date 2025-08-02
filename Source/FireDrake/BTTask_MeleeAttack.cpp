#include "BTTask_MeleeAttack.h"
#include "AIController.h"
#include "DwarfCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = "Melee Attack";
	bNotifyTick = true;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MeleeAttack, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	ADwarfCharacter* DwarfCharacter = Cast<ADwarfCharacter>(AIController->GetPawn());
	if (!DwarfCharacter)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Target)
	{
		return EBTNodeResult::Failed;
	}

	float DistanceToTarget = FVector::Dist(DwarfCharacter->GetActorLocation(), Target->GetActorLocation());
	if (DistanceToTarget > DwarfCharacter->MeleeRange)
	{
		return EBTNodeResult::Failed;
	}

	DwarfCharacter->PerformMeleeAttack();
	AttackTimer = 0.0f;
	bHasDealtDamage = false;

	return EBTNodeResult::InProgress;
}

void UBTTask_MeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AttackTimer += DeltaSeconds;

	if (!bHasDealtDamage && AttackTimer >= DamageDelay)
	{
		DealDamage(OwnerComp);
		bHasDealtDamage = true;
	}

	if (AttackTimer >= AttackDuration)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_MeleeAttack::DealDamage(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	ADwarfCharacter* DwarfCharacter = Cast<ADwarfCharacter>(AIController->GetPawn());
	if (!DwarfCharacter)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Target)
	{
		return;
	}

	float DistanceToTarget = FVector::Dist(DwarfCharacter->GetActorLocation(), Target->GetActorLocation());
	if (DistanceToTarget <= DwarfCharacter->MeleeRange)
	{
		FPointDamageEvent DamageEvent;
		DamageEvent.DamageTypeClass = UDamageType::StaticClass();
		DamageEvent.HitInfo.Location = Target->GetActorLocation();
		DamageEvent.HitInfo.ImpactPoint = Target->GetActorLocation();
		DamageEvent.ShotDirection = (Target->GetActorLocation() - DwarfCharacter->GetActorLocation()).GetSafeNormal();

		Target->TakeDamage(DwarfCharacter->MeleeDamage, DamageEvent, AIController, DwarfCharacter);
	}
}