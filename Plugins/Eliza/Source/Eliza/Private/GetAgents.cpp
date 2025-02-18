// Copyright Crucible Networks Ltd 2025. All Rights Reserved.


#include "GetAgents.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UGetAgents* UGetAgents::GetAgents(UElizaInstance* _ElizaInstanceOverride)
{
	UGetAgents* BlueprintNode = NewObject<UGetAgents>();
	BlueprintNode->ElizaInstanceOverride = _ElizaInstanceOverride;
	return BlueprintNode;
}

void UGetAgents::Activate()
{
	FString requestURL;

	TArray<TPair<FString, FString>> Headers;

	if (ElizaInstanceOverride) {
		if (ElizaInstanceOverride->ElizaInstance.APIType == EElizaAPIType::GenericEliza) {
			requestURL = ElizaInstanceOverride->ElizaInstance.LocationURL + "/Agents";
		}
		if (ElizaInstanceOverride->ElizaInstance.APIType == EElizaAPIType::Fleek) {
			requestURL = "https://api.fleek.xyz/api/v1/ai-agents/" + ElizaInstanceOverride->ElizaInstance.FleekAgentId + "/api/Agents";
			Headers.Add(TPair<FString, FString>{"x-api-key", "" + ElizaInstanceOverride->ElizaInstance.FleekAPIKey});
		}
	}
	else {
		requestURL = UElizaHttpHelperLibrary::GetElizaStarterUrl() + "/Agents";
	}

	UElizaHttpHelperLibrary::ExecuteHttpRequest<UGetAgents>(
		this,
		&UGetAgents::GetAgents_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers
	);
}

void UGetAgents::GetAgents_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded) {
		TSharedPtr<FJsonValue> JsonValue;
		FString ResponseString = HttpResponse->GetContentAsString();
		UE_LOG(LogEliza, Display, TEXT("Get Agents response: %s"), *ResponseString);
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			TArray<FAgentInfo> Agents;
			auto AgentsArray = JsonValue->AsObject()->GetArrayField(TEXT("agents"));
			for (int i = 0; i < AgentsArray.Num(); i++) {
				auto Agent = AgentsArray[i]->AsObject();
				Agents.Add(FAgentInfo(Agent->GetStringField(TEXT("id")), Agent->GetStringField(TEXT("name"))));
			}
			OnGetAgentsCompleted.Broadcast(true, Agents);

			return;
		}
	}

	OnGetAgentsCompleted.Broadcast(false, TArray<FAgentInfo>());
	return;
}
