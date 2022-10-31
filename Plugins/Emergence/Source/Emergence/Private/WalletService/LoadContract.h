// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "EmergenceDeployment.h"
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
	 * @warn THIS NODE IS DEPRECATED. This is now handled internally by ReadMethod and WriteMethod. You no longer need to call this. Please remove this node from your code ASAP.
	 */
	UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction, DeprecationMessage="This is now handled internally by ReadMethod and WriteMethod. You no longer need to call this. Please remove this node from your code ASAP.", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static ULoadContract* LoadContract(const UObject* WorldContextObject, UEmergenceDeployment* DeployedContract);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadContractCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, meta = (DeprecatedProperty, DeprecationMessage="This is now handled internally by ReadMethod and WriteMethod. You no longer need to call this. Please remove this node from your code ASAP."))
	FOnLoadContractCompleted OnLoadContractCompleted;
private:
	void LoadContract_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	UEmergenceDeployment* DeployedContract;
};
