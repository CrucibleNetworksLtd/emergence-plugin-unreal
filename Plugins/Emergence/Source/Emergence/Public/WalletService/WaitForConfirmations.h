// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "EmergenceDeployment.h"
#include "WalletService/GetTransactionStatus.h"
#include "WaitForConfirmations.generated.h"

UCLASS()
class EMERGENCE_API UWaitForConfirmations : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Lets you wait for the confirmation.
	 * @param TrasactionHash Hash of the transaction.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UWaitForConfirmations* WaitForConfirmations(UObject* WorldContextObject, FString TransactionHash, UEmergenceChain* BlockchainOverride, int NumberOfConfirmations = 5);

	virtual void Activate() override;

	UFUNCTION()
	void ActivateByTimer();

	UFUNCTION()
	void TransactionStatusReturns(FEmergenceTransaction Transaction, EErrorCode StatusCode);

	UFUNCTION()
	void WaitThenGetStatus();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaitForConfirmationsCompleted, FEmergenceTransaction, Transaction, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnWaitForConfirmationsCompleted OnWaitForConfirmationsCompleted;
private:
	UObject* WorldContextObject;
	FString TransactionHash;
	UEmergenceChain* BlockchainOverride;
	int NumberOfConfirmations;
	FTimerHandle TimerHandle;
};
