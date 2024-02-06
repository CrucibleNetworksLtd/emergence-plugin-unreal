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
	//Remove blueprintcallable when testing finishes
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence Internal|Futureverse Methods")
	static USendARTMAsync* SendARTMAsync(UObject* WorldContextObject, FString Message, TArray<FEmergenceFutureverseARTMOperation> ARTMOperations);

	virtual void Activate() override;

	virtual void Cancel();

	virtual bool IsActive() const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSendARTMCompleted, FString, TransactionHash, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnSendARTMCompleted OnSendARTMCompleted;

private:
	FString ConstructedMessage;
	FString _MessageToUser;
	TArray<FEmergenceFutureverseARTMOperation> _ARTMOperations;

	FHttpRequestPtr GetNonceRequest;
	FHttpRequestPtr RequestToSignRequest;
	FHttpRequestPtr SendMutationRequest;

	UFUNCTION()
	void OnRequestToSignCompleted(FString SignedMessage, EErrorCode StatusCode);
};
