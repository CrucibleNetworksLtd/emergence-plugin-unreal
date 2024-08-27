// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "CustodialLogin.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UCustodialLogin : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:

	//@TODO split this into its own module
	//@TODO split this into login and transaction
	//@TODO add states

	bool _isServerStarted = false;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Custodial Login")
	static UCustodialLogin* CustodialLogin(UObject* WorldContextObject);

	UFUNCTION()
	FString Base64UrlEncodeNoPadding(FString Input);

	void GetTokensRequest_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void WriteMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void SendTransaction_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAvatarByIdCompleted, const FString, Avatar, FString, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnAvatarByIdCompleted OnAvatarByIdCompleted;

	UPROPERTY()
	FString clientid = "8XPY4Vnc6BBn_4XNBYk0P"; //@TODO get an actual client ID
	
	UPROPERTY()
	FString code; //@TODO make this a random string
		
	UPROPERTY()
	FString EncodedSig; 
	
	UPROPERTY()
	FString state = "Zx9j1PwATnAODKjd"; //@TODO make this a random string

	UPROPERTY()
	FString FVUserAddress;

	void Activate() override;
	bool HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete);
	bool HandleRequestCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	void RequestPrint(const FHttpServerRequest& Request, bool PrintBody = true);
	
	UFUNCTION()
	static bool DecodeJwt(FString input, TMap<FString, FString>& Output);
};
