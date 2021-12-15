// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "ReadMethod.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UReadMethod : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Emergence Requests|Persona Server")
	static UReadMethod* ReadMethod(const UObject* WorldContextObject, FString ContractAddress, FString MethodName, TArray<FString> Content);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReadMethodCompleted, FString, Response, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnReadMethodCompleted OnReadMethodCompleted;
private:
	void ReadMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString ContractAddress;
	FString MethodName;
	TArray<FString> Content;
};
