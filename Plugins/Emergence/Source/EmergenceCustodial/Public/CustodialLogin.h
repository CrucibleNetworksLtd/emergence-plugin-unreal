// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "CustodialLogin.generated.h"

//This only exists because I can't output a tmap from a dynamic multicast deligate
//@TODO Hide this from blueprint when we're done
USTRUCT(BlueprintType)
struct FEmergenceCustodialLoginOutput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FString> TokenData;

	FEmergenceCustodialLoginOutput() {};

	FEmergenceCustodialLoginOutput(TMap<FString, FString> _TokenData) {
		TokenData = _TokenData;
	};
};

UCLASS()
class EMERGENCECUSTODIAL_API UCustodialLogin : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:

	//@TODO split this into its own module
	//@TODO split this into login and transaction
	//@TODO add states

	UPROPERTY()
	bool _isServerStarted = false;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Custodial Login")
	static UCustodialLogin* CustodialLogin(UObject* WorldContextObject);

	UFUNCTION()
	FString Base64UrlEncodeNoPadding(FString Input);

	void GetTokensRequest_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetEncodedPayload_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void SendTransaction_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	TUniquePtr<FHttpServerResponse> GetHttpPage();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustodialLoginCompleted, FEmergenceCustodialLoginOutput, TokenData, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnCustodialLoginCompleted OnLoginCompleted;

	UPROPERTY()
	FString clientid = "8XPY4Vnc6BBn_4XNBYk0P"; //@TODO get an actual client ID
	
	UPROPERTY()
	FString code; //@TODO make this a random string
	
	UPROPERTY()
	FString state = "Zx9j1PwATnAODKjd"; //@TODO make this a random string

	void Activate() override;
	bool HandleAuthRequestCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	void RequestPrint(const FHttpServerRequest& Request, bool PrintBody = true);
	
	UFUNCTION()
	static bool DecodeJwt(FString input, TMap<FString, FString>& Output);
};
