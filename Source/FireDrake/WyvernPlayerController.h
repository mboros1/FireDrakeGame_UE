#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WyvernPlayerController.generated.h"

UCLASS()
class FIREDRAKE_API AWyvernPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWyvernPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};