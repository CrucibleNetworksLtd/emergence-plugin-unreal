// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "CustodialSignMessage.generated.h"

//This class should only be called by Request To Sign
UCLASS(meta=(HasDedicatedAsyncNode))
class EMERGENCE_API UCustodialSignMessage : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Custodial Login")
	static UCustodialSignMessage* CustodialSignMessage(UObject* WorldContextObject, FString FVCustodialEOA, FString Message);

	void BeginDestroy() override;

	void LaunchSignMessageURL();

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

	static bool _isServerStarted;

	static FHttpRouteHandle RouteHandle;
};
