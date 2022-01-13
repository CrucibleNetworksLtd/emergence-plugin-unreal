// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "GetBalance.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UGetBalance : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Wallet Requests")
	static UGetBalance* GetBalance(const UObject* WorldContextObject, FString Address);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetBalanceCompleted, FString, Balance, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetBalanceCompleted OnGetBalanceCompleted;
private:
	void GetBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString Address;
};
