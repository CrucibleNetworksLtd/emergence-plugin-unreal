// Copyright Crucible Networks Ltd 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Interfaces/IHttpRequest.h"
#include "GetVirtualsAgents.generated.h"

USTRUCT(BlueprintType)
struct FVirtualsAgent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Agent")
	FString entityId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Agent")
	FString name;

	FVirtualsAgent() {};

	FVirtualsAgent(FString _EntityId, FString _Name) {
		entityId = _EntityId;
		name = _Name;
	};
};

UCLASS()
class ELIZA_API UGetVirtualsAgents : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	//@TODO document this
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", KeyWords = "Eliza Character Agent"), Category = "Eliza")
	static UGetVirtualsAgents* GetVirtualsAgents(FString WorldId);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetVirtualsAgentsCompleted, bool, Success, const TArray<FVirtualsAgent>&, Agents);

	UPROPERTY(BlueprintAssignable)
	FOnGetVirtualsAgentsCompleted OnGetVirtualsAgentsCompleted;
private:
	void GetVirtualsAgents_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString WorldId;
};
