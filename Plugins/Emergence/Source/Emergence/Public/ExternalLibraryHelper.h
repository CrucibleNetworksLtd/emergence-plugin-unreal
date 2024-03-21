// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExternalLibraryHelper.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UExternalLibraryHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static FString WideToStringCleanup(char* WeirdChars);
};
