// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BigIntFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UBigIntFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure)
	static FString HexToDec(const FString& HexValue);
};
