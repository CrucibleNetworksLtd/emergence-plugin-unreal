// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "GetUserUnlockedAvatars.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UGetUserUnlockedAvatars : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Emergence Requests|Persona Server")
	static UGetUserUnlockedAvatars* GetUserUnlockedAvatars(const UObject* WorldContextObject);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetUserUnlockedAvatarsCompleted, FEmergenceAvatarListResponse, Response, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetUserUnlockedAvatarsCompleted OnGetUserUnlockedAvatarsCompleted;
private:
	void GetUserUnlockedAvatars_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
};
