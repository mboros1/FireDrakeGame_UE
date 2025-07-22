#include "WyvernPlayerController.h"

AWyvernPlayerController::AWyvernPlayerController()
{
	bShowMouseCursor = false;
}

void AWyvernPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Set input mode to game only
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void AWyvernPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}