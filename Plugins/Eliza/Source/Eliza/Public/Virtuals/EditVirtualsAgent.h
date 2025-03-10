// Copyright Crucible Networks Ltd 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Interfaces/IHttpRequest.h"
#include "EditVirtualsAgent.generated.h"


UCLASS()
class ELIZA_API UEditVirtualsAgent : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	//@TODO document this
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", KeyWords = "Eliza Character Agent"), Category = "Eliza")
	static UEditVirtualsAgent* EditVirtualsAgent(FString EntityId, FString Goal, FString Description);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEditVirtualsAgentCompleted, bool, Success);

	UPROPERTY(BlueprintAssignable)
	FOnEditVirtualsAgentCompleted OnEditVirtualsAgentCompleted;
private:
	void EditVirtualsAgent_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString EntityId, Goal, Description;
};
