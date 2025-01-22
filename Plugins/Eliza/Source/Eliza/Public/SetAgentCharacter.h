// Copyright Crucible Networks Ltd 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Interfaces/IHttpRequest.h"
#include "AgentDetails.h"
#include "SetAgentCharacter.generated.h"


UCLASS()
class ELIZA_API USetAgentCharacter : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Eliza")
	static USetAgentCharacter* SetAgentCharacter(FAgentDetails AgentDetails);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetAgentCharacterCompleted, bool, Success, FAgentDetails, Agent);

	UPROPERTY(BlueprintAssignable)
	FOnSetAgentCharacterCompleted OnSetAgentCharacterCompleted;
private:
	void SetAgentCharacter_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FAgentDetails AgentDetails;
};
