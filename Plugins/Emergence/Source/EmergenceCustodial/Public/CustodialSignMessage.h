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
class EMERGENCECUSTODIAL_API UCustodialSignMessage : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:

	UPROPERTY()
	bool _isServerStarted = false;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Custodial Login")
	static UCustodialSignMessage* CustodialSignMessage(UObject* WorldContextObject, FString FVCustodialEOA);

	bool HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAvatarByIdCompleted, const FString, Avatar, FString, StatusCode);

	TUniquePtr<FHttpServerResponse> GetHttpPage();

	UPROPERTY(BlueprintAssignable)
	FOnAvatarByIdCompleted OnAvatarByIdCompleted;

	UPROPERTY()
	FString FVCustodialEOA;

	void Activate() override;

	void RequestPrint(const FHttpServerRequest& Request, bool PrintBody = true);
};
