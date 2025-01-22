// Copyright Crucible Networks Ltd 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Interfaces/IHttpRequest.h"
#include "AgentDetails.h"
#include "GetAgentCharacter.generated.h"


UCLASS()
class ELIZA_API UGetAgentCharacter : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Eliza")
	static UGetAgentCharacter* GetAgentCharacter(FString AgentId);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetAgentCharacterCompleted, bool, Success, FAgentDetails, Agent);

	UPROPERTY(BlueprintAssignable)
	FOnGetAgentCharacterCompleted OnGetAgentCharacterCompleted;
private:
	void GetAgentCharacter_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString AgentId;
};
