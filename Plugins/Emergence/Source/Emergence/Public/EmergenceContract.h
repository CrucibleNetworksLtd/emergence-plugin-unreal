// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergenceContract.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceContractMethod
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString MethodName;

	FEmergenceContractMethod() {};

	FEmergenceContractMethod(FString _MethodName) {
		MethodName = _MethodName;
	};
};

UCLASS(BlueprintType, CollapseCategories)
class EMERGENCE_API UEmergenceContract : public UObject
{
	GENERATED_BODY()
	
public:
	UEmergenceContract() {};

	UEmergenceContract(FString _ABI);

	UFUNCTION(BlueprintPure)
	static UEmergenceContract* CreateEmergenceContract(FString _ABI);
	
	UPROPERTY(EditAnywhere)
	FString ABI;

	UPROPERTY(EditAnywhere)
	TArray<FEmergenceContractMethod> Methods;

	UFUNCTION(CallInEditor)
	void FindMethods();
};
