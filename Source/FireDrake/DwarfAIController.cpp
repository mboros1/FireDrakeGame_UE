#include "DwarfAIController.h"
#include "DwarfCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ADwarfAIController::ADwarfAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionComponent(*AIPerceptionComponent);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.0f;
	SightConfig->LoseSightRadius = 1500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->SetMaxAge(5.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	DamageConfig->SetMaxAge(5.0f);

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->ConfigureSense(*DamageConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ADwarfAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ADwarfAIController::OnTargetPerceptionUpdated);
		AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ADwarfAIController::OnPerceptionUpdated);
	}
}

void ADwarfAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ADwarfCharacter* DwarfCharacter = Cast<ADwarfCharacter>(InPawn);
	if (DwarfCharacter && BehaviorTreeAsset)
	{
		UBlackboardComponent* BBComponent = nullptr;
		UseBlackboard(BlackboardAsset, BBComponent);
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void ADwarfAIController::OnUnPossess()
{
	Super::OnUnPossess();

	UBehaviorTreeComponent* BehaviorTreeComp = GetBrainComponent() ? Cast<UBehaviorTreeComponent>(GetBrainComponent()) : nullptr;
	if (BehaviorTreeComp)
	{
		BehaviorTreeComp->StopTree();
	}
}

void ADwarfAIController::SetTargetActor(AActor* NewTarget)
{
	TargetActor = NewTarget;
	UpdateBlackboard();
}

bool ADwarfAIController::HasLineOfSightToTarget() const
{
	if (!TargetActor || !GetPawn())
	{
		return false;
	}

	FHitResult HitResult;
	FVector StartLocation = GetPawn()->GetActorLocation();
	FVector EndLocation = TargetActor->GetActorLocation();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetPawn());
	QueryParams.AddIgnoredActor(TargetActor);

	return !GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams);
}

void ADwarfAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			ACharacter* Character = Cast<ACharacter>(Actor);
			if (Character && !Character->IsA<ADwarfCharacter>())
			{
				SetTargetActor(Actor);
			}
		}
		else
		{
			if (TargetActor == Actor)
			{
				SetTargetActor(nullptr);
			}
		}
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			// For damage stimulus, the instigator is the actor who caused the damage
			AActor* DamageInstigator = Actor; // In damage perception, the Actor parameter is the instigator
			ACharacter* Character = Cast<ACharacter>(DamageInstigator);
			if (Character && !Character->IsA<ADwarfCharacter>())
			{
				SetTargetActor(Character);
			}
		}
	}
}

void ADwarfAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo Info;
		AIPerceptionComponent->GetActorsPerception(Actor, Info);

		for (const FAIStimulus& Stimulus : Info.LastSensedStimuli)
		{
			OnTargetPerceptionUpdated(Actor, Stimulus);
		}
	}
}

void ADwarfAIController::UpdateBlackboard()
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	if (BBComponent)
	{
		BBComponent->SetValueAsObject(FName("TargetActor"), TargetActor);
		BBComponent->SetValueAsBool(FName("HasLineOfSight"), HasLineOfSightToTarget());
	}
}