// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "CreateKeyStore.generated.h"

/**
 *
 */
UCLASS()
class EMERGENCE_API UCreateKeyStore : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Wallet Requests")
	static UCreateKeyStore *CreateKeyStore(const UObject *WorldContextObject, const FString &PrivateKey, const FString &Password, const FString &PublicKey, const FString &Path);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreateKeyStoreCompleted, FString, Response, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnCreateKeyStoreCompleted OnCreateKeyStoreCompleted;

private:
	FString PrivateKey;
	FString Password;
	FString PublicKey;
	FString Path;
	void CreateKeyStore_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject *WorldContextObject;
};
