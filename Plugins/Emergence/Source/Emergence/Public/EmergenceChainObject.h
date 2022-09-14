// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergenceChainObject.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, CollapseCategories)
class EMERGENCE_API UEmergenceChain : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FString NodeURL;

	UPROPERTY(EditAnywhere)
	FString Symbol;

	UFUNCTION()
	static UEmergenceChain* GetEmergenceChainDataFromConfig(UObject* Outer);
};
