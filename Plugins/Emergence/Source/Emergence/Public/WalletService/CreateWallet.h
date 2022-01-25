// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "CreateWallet.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UCreateWallet : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Wallet Requests")
	static UCreateWallet *CreateWallet(const UObject *WorldContextObject, const FString &Path, const FString &Password);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreateWalletCompleted, FString, Response, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnCreateWalletCompleted OnCreateWalletCompleted;

private:
	FString Path;
	FString Password;
	void CreateWallet_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject *WorldContextObject;
};
