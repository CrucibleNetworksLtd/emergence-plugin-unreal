// Copyright Crucible Networks Ltd 2025. All Rights Reserved.


#include "GetAgentCharacter.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UGetAgentCharacter* UGetAgentCharacter::GetAgentCharacter(FString _AgentId, UElizaInstance* _ElizaInstanceOverride)
{
	UGetAgentCharacter* BlueprintNode = NewObject<UGetAgentCharacter>();
	BlueprintNode->AgentId = _AgentId;
	BlueprintNode->ElizaInstanceOverride = _ElizaInstanceOverride;
	return BlueprintNode;
}

void UGetAgentCharacter::Activate()
{
	FString requestURL;

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	if (ElizaInstanceOverride) {
		if (ElizaInstanceOverride->ElizaInstance.APIType == EElizaAPIType::GenericEliza) {
			requestURL = ElizaInstanceOverride->ElizaInstance.LocationURL + "/Agents/" + AgentId;
		}
		if (ElizaInstanceOverride->ElizaInstance.APIType == EElizaAPIType::Fleek) {
			requestURL = "https://api.fleek.xyz/api/v1/ai-agents/" + ElizaInstanceOverride->ElizaInstance.FleekAgentId + "/api/Agents/" + AgentId;
			Headers.Add(TPair<FString, FString>{"x-api-key", "" + ElizaInstanceOverride->ElizaInstance.FleekAPIKey});
		}
	}
	else {
		requestURL = UElizaHttpHelperLibrary::GetElizaStarterUrl() + "/Agents/" + AgentId;
	}
	
	UElizaHttpHelperLibrary::ExecuteHttpRequest<UGetAgentCharacter>(
		this,
		&UGetAgentCharacter::GetAgentCharacter_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers
	);
}

void UGetAgentCharacter::GetAgentCharacter_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded) {
		TSharedPtr<FJsonValue> JsonValue;
		FString ResponseString = HttpResponse->GetContentAsString();
		UE_LOG(LogEliza, Display, TEXT("Get Agent Character response: %s"), *ResponseString);
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {

			FAgentDetails AgentDetails = FAgentDetails(HttpResponse->GetContentAsString());
			OnGetAgentCharacterCompleted.Broadcast(true, AgentDetails);

			return;
		}
	}

	OnGetAgentCharacterCompleted.Broadcast(false, FAgentDetails());
	return;
}
