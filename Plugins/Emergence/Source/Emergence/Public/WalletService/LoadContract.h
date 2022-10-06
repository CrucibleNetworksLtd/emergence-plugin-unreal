// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "LoadContract.generated.h"

UCLASS()
class EMERGENCE_API ULoadContract : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Loads a contract into the local server's memory.
	 * @param ContractAddress Address of the contract.
	 * @param ABI The Application Binary Interface of the given contract.
	 * @param Blockchain The blockchain this contract is on. Leaving this blank will set it to the "default" blockchain. You only need to worry about using this if you are making use of multiple blockchains, as it is specifically designed to avoid the situation that two blockchains have a contract with the same address.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static ULoadContract* LoadContract(const UObject* WorldContextObject, FString ContractAddress, FString ABI, FString Blockchain = "");

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadContractCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnLoadContractCompleted OnLoadContractCompleted;
private:
	void LoadContract_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString ContractAddress;
	FString ABI;
	FString Blockchain;
};
