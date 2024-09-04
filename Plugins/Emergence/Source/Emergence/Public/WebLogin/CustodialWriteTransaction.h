// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "CustodialWriteTransaction.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UCustodialWriteTransaction : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:

	//@TODO split this into its own module
	//@TODO split this into login and transaction
	//@TODO add states

	UPROPERTY()
	bool _isServerStarted = false;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Custodial Login")
	static UCustodialWriteTransaction* CustodialWriteTransaction(UObject* WorldContextObject, FString FVCustodialEOA, UEmergenceDeployment* DeployedContract, FString Method);

	void GetEncodedPayload_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	bool HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete);

	void SendTransaction_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAvatarByIdCompleted, const FString, TransactionHash, EErrorCode, StatusCode);

	TUniquePtr<FHttpServerResponse> GetHttpPage();

	UPROPERTY(BlueprintAssignable)
	FOnAvatarByIdCompleted OnAvatarByIdCompleted;

	UPROPERTY()
	UEmergenceDeployment* DeployedContract;

	UPROPERTY()
	FString Method;
		
	UPROPERTY()
	FString EncodedSig; 

	UPROPERTY()
	FString FVCustodialEOA;

	UPROPERTY()
	FString TransactionNonce;

	UPROPERTY()
	FString UnsignedTransaction;

	void Activate() override;

	void RequestPrint(const FHttpServerRequest& Request, bool PrintBody = true);
};
