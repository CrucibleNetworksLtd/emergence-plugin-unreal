// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "Transaction.h"
#include "EmergenceChainObject.h"
#include "GetTransactionStatus.generated.h"

UCLASS()
class EMERGENCE_API UGetTransactionStatus : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the status of the transaction.
	 * @param TransactionHash Hash of the transaction.
	 * @param BlockchainOverride Blockchain to get the transaction status from. Leave blank for blockchain from Emergence project settings.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UGetTransactionStatus* GetTransactionStatus(const UObject* WorldContextObject, FString TransactionHash, UEmergenceChain* BlockchainOverride);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTransactionStatusCompleted, FEmergenceTransaction, Transaction, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetTransactionStatusCompleted OnGetTransactionStatusCompleted;
private:
	void GetTransactionStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString TransactionHash;
	UEmergenceChain* Blockchain;
};
