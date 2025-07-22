#include "FireDrakeGameMode.h"
#include "WyvernCharacter.h"
#include "WyvernPlayerController.h"

AFireDrakeGameMode::AFireDrakeGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AFireDrakeGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AFireDrakeGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
