// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ErrorCodeFunctionLibrary.h"
#include "EmergenceUEExampleGameMode.generated.h"

UCLASS(minimalapi)
class AEmergenceUEExampleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEmergenceUEExampleGameMode();

	UFUNCTION(BlueprintCallable)
	void PerformCustodialLogin();

	UFUNCTION()
	void OnLoginFinalized(EErrorCode StatusCode);
};



