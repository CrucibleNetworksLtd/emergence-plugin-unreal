// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "CustodialSignMessage.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UCustodialSignMessage : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:

	UPROPERTY()
	bool _isServerStarted = false;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Custodial Login")
	static UCustodialSignMessage* CustodialSignMessage(UObject* WorldContextObject, FString FVCustodialEOA, FString Message);

	bool HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustodialSignMessageComplete, const FString, SignedMessage, EErrorCode, StatusCode);

	TUniquePtr<FHttpServerResponse> GetHttpPage();

	UPROPERTY(BlueprintAssignable)
	FOnCustodialSignMessageComplete OnCustodialSignMessageComplete;

	UPROPERTY()
	FString FVCustodialEOA;

	UPROPERTY()
	FString Message;

	void Activate() override;

	void RequestPrint(const FHttpServerRequest& Request, bool PrintBody = true);
};
