// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AvatarServiceLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UAvatarServiceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Avatar Service")
	static bool GetEmergencePreferredNodeURL(FString Blockchain, FString& URL);
};
