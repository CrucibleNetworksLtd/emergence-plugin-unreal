// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "LoadAccount.generated.h"

/**
 *
 */
UCLASS()
class EMERGENCE_API ULoadAccount : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Wallet Requests")
	static ULoadAccount *LoadAccount(
		const UObject *WorldContextObject,
		const FString &Name,
		const FString &Password,
		const FString &Path,
		const FString &NodeURL);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadAccountCompleted, FString, Response, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnLoadAccountCompleted OnLoadAccountCompleted;

private:
	FString Name;
	FString Password;
	FString Path;
	FString NodeURL;
	void LoadAccount_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject *WorldContextObject;
};
