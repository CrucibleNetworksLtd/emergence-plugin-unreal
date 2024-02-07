// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARTMBuilderLibrary.generated.h"

UENUM(BlueprintType)
enum class EEmergenceFutureverseARTMOperationType : uint8 {
	CREATELINK       UMETA(DisplayName = "Create Asset Link"),
	DELETELINK        UMETA(DisplayName = "Delete Asset Link"),
	NONE
};

USTRUCT(BlueprintType)
struct FEmergenceFutureverseARTMOperation
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Futureverse|ATRM Operation")
	EEmergenceFutureverseARTMOperationType OperationType = EEmergenceFutureverseARTMOperationType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse|ATRM Operation")
	FString Slot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse|ATRM Operation")
	FString LinkA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse|ATRM Operation")
	FString LinkB;

	
};

UCLASS()
class EMERGENCE_API UARTMBuilderLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//ADDRESS IS EOA ADDRESS / MUST BE ETH CHECKSUMMED
	UFUNCTION(BlueprintPure)
	static FString GenerateARTM(FString Message, TArray<FEmergenceFutureverseARTMOperation> ARTMOperations, FString Address, FString Nonce);

	/*UFUNCTION(BlueprintPure, Category = "Emergence|Futurepass|ARTM Operation", meta = (BlueprintThreadSafe))
	static FEmergenceFutureverseARTMOperation MakeEmergenceFutureverseARTMCreateAssetLink(FString Slot, FString LinkA, FString LinkB)
	{
		FEmergenceFutureverseARTMOperation Operation;
		Operation.LinkA = LinkA;
		Operation.LinkB = LinkB;
		Operation.Slot = Slot;
		Operation.OperationType = EEmergenceFutureverseARTMOperationType::CREATELINK;
		return Operation;
	}

	UFUNCTION(BlueprintPure, Category = "Emergence|Futurepass|ARTM Operation", meta = (BlueprintThreadSafe))
	static FEmergenceFutureverseARTMOperation MakeEmergenceFutureverseARTMDeleteAssetLink(FString Slot, FString LinkA, FString LinkB)
	{
		FEmergenceFutureverseARTMOperation Operation;
		Operation.LinkA = LinkA;
		Operation.LinkB = LinkB;
		Operation.Slot = Slot;
		Operation.OperationType = EEmergenceFutureverseARTMOperationType::DELETELINK;
		return Operation;
	}*/
};
