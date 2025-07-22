#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FireDrakeGameMode.generated.h"

UCLASS()
class FIREDRAKE_API AFireDrakeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFireDrakeGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
