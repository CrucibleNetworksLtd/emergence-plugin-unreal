// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "DeletePersonaByID.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UDeletePersonaByID : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Emergence|Emergence Requests|Persona Server")
	static UDeletePersonaByID* DeletePersonaByID(const FString& PersonaID);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeletePersonaByIDCompleted, FEmergencePersona, Response, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnDeletePersonaByIDCompleted OnDeletePersonaByIDCompleted;
private:
	void DeletePersonaByID_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString PersonaID;
};
