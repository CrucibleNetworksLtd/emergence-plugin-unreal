// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "EmergenceInventoryServiceStructs.h"
#include "InventoryByOwner.generated.h"


UCLASS()
class EMERGENCE_API UInventoryByOwner : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Emergence Requests|Inventory Service")
	static UInventoryByOwner* InventoryByOwner(const UObject* WorldContextObject, const FString& Address, const FString& Network);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryByOwnerCompleted, FEmergenceInventory, Inventory, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnInventoryByOwnerCompleted OnInventoryByOwnerCompleted;
private:
	void InventoryByOwner_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString Address, Network;
	const UObject* WorldContextObject;
};
