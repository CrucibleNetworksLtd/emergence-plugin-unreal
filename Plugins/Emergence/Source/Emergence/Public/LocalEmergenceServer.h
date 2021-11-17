// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
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
	static void LaunchLocalServerProcess();

	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Requests")
	static void KillLocalServerProcess();
};
