// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "RequestToSign.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API URequestToSign : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Validates an access token. Usually used server-side to authenticate players.
	 * @param AccessToken The access token to authenticate.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Wallet Requests")
	static URequestToSign* RequestToSign(const UObject* WorldContextObject, const FString& MessageToSign);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestToSignCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnRequestToSignCompleted OnRequestToSignCompleted;
private:
	void RequestToSign_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString MessageToSign;
};
