// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "WriteMethod.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UWriteMethod : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Emergence Requests|Persona Server")
	static UWriteMethod* WriteMethod(const UObject* WorldContextObject, FString ContractAddress, FString MethodName, TArray<FString> Content);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWriteMethodCompleted, FString, Response, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnWriteMethodCompleted OnWriteMethodCompleted;
private:
	void WriteMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString ContractAddress;
	FString MethodName;
	TArray<FString> Content;
};
