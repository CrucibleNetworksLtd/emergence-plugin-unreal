// Copyright Crucible Networks Ltd 2025. All Rights Reserved.


#include "GetVirtualsAgents.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UGetVirtualsAgents* UGetVirtualsAgents::GetVirtualsAgents(FString WorldId)
{
	UGetVirtualsAgents* BlueprintNode = NewObject<UGetVirtualsAgents>();
	BlueprintNode->WorldId = WorldId;
	return BlueprintNode;
}

void UGetVirtualsAgents::Activate()
{
	FString requestURL = "https://blueprint-serve-8305v2t4v-narraio.vercel.app/virtual-agents/list";

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	UElizaHttpHelperLibrary::ExecuteHttpRequest<UGetVirtualsAgents>(
		this,
		&UGetVirtualsAgents::GetVirtualsAgents_HttpRequestComplete,
		requestURL,
		"POST",
		60.0F,
		Headers,
		"{\"worldId\": \"" + WorldId + "\"}"
	);
}

void UGetVirtualsAgents::GetVirtualsAgents_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded) {
		TSharedPtr<FJsonValue> JsonValue;
		FString ResponseString = HttpResponse->GetContentAsString();
		UE_LOG(LogEliza, Display, TEXT("Get Agents response: %s"), *ResponseString);
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			TArray<FVirtualAgent> Agents;
			auto AgentsArray = JsonValue->AsArray();
			for (int i = 0; i < AgentsArray.Num(); i++) {
				auto Agent = AgentsArray[i]->AsObject();
				Agents.Add(FVirtualAgent(Agent->GetStringField(TEXT("entityId")), Agent->GetStringField(TEXT("name"))));
			}
			OnGetVirtualsAgentsCompleted.Broadcast(true, Agents);

			return;
		}
	}

	OnGetVirtualsAgentsCompleted.Broadcast(false, TArray<FVirtualAgent>());
	return;
}
