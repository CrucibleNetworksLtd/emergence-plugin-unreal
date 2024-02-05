// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceCancelableAsyncBase.h"
#include "Futurepass\ARTMBuilderLibrary.h"
#include "ErrorCodeFunctionLibrary.h"
#include "SendARTMAsync.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API USendARTMAsync : public UEmergenceCancelableAsyncBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Futureverse Methods")
	static USendARTMAsync* SendARTMAsync(UObject* WorldContextObject, FString Message, TArray<FEmergenceFutureverseARTMOperation> ARTMOperations);

	virtual void Activate() override;

private:
	FString _Message;
	TArray<FEmergenceFutureverseARTMOperation> _ARTMOperations;

	FHttpRequestPtr GetNonceRequest;
	FHttpRequestPtr RequestToSignRequest;

	UFUNCTION()
	void OnRequestToSignCompleted(FString SignedMessage, EErrorCode StatusCode);
};
