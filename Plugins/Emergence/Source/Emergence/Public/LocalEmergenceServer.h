// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Emergence.h"
#include "LocalEmergenceServer.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API ULocalEmergenceServer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Requests")
	static void LaunchLocalServerProcess(bool LaunchHidden = true);

	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Requests")
	static void KillLocalServerProcess();
};
