// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "UpdatePersona.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UUpdatePersona : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Emergence|Emergence Requests|Persona Server")
	static UUpdatePersona* UpdatePersona(FEmergencePersona Persona);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdatePersonaCompleted, FEmergencePersonaResponse, Response, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnUpdatePersonaCompleted OnUpdatePersonaCompleted;
private:
	void UpdatePersona_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FEmergencePersona Persona;
};
