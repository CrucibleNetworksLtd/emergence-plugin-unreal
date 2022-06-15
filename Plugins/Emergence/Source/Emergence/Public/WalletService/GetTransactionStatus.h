// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "Transaction.h"
#include "GetTransactionStatus.generated.h"

UCLASS()
class EMERGENCE_API UGetTransactionStatus : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Calls a "read" method on the given contract.
	 * @param ContractAddress Address of the contract.
	 * @param MethodName The method to call.
	 * @param Content The parameters to call the method with.
	 * @param LocalAccountName The local account to call the method with. Leave blank to call with the WalletConnect'd wallet.
	 * @warning Make sure the local server already knows about the contract by calling LoadContract first!
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Contract Requests")
	static UGetTransactionStatus* GetTransactionStatus(const UObject* WorldContextObject, FString TransactionHash, FString NodeURL);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTransactionStatusCompleted, FEmergenceTransaction, Transaction, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetTransactionStatusCompleted OnGetTransactionStatusCompleted;
private:
	void GetTransactionStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString TransactionHash;
	FString NodeURL;
};
