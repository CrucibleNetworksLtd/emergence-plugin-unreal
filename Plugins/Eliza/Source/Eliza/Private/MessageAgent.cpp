// Copyright Crucible Networks Ltd 2025. All Rights Reserved.


#include "MessageAgent.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UMessageAgent* UMessageAgent::MessageAgent(FString _AgentId, FString _Message, UElizaInstance* _ElizaInstanceOverride)
{
	UMessageAgent* BlueprintNode = NewObject<UMessageAgent>();
	BlueprintNode->AgentId = _AgentId;
	BlueprintNode->Message = _Message;
	BlueprintNode->ElizaInstanceOverride = _ElizaInstanceOverride;
	return BlueprintNode;
}

void UMessageAgent::Activate()
{
	FString requestURL;

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	if (ElizaInstanceOverride) {
		if (ElizaInstanceOverride->ElizaInstance.APIType == EElizaAPIType::GenericEliza) {
			requestURL = ElizaInstanceOverride->ElizaInstance.LocationURL + "/" + AgentId + "/message";
		}
		if (ElizaInstanceOverride->ElizaInstance.APIType == EElizaAPIType::Fleek) {
			requestURL = "https://api.fleek.xyz/api/v1/ai-agents/" + ElizaInstanceOverride->ElizaInstance.FleekAgentId + "/api/" + AgentId + "/message";
			Headers.Add(TPair<FString, FString>{"x-api-key", "" + ElizaInstanceOverride->ElizaInstance.FleekAPIKey});
		}
	}
	else {
		requestURL = UElizaHttpHelperLibrary::GetElizaStarterUrl() + "/" + AgentId + "/message";
	}

	TSharedPtr<FJsonObject> BodyContentJsonObject = MakeShareable(new FJsonObject);
	BodyContentJsonObject->SetStringField(TEXT("text"), Message);
	FString JsonOutput;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonOutput);
	FJsonSerializer::Serialize(BodyContentJsonObject.ToSharedRef(), Writer);

	UElizaHttpHelperLibrary::ExecuteHttpRequest<UMessageAgent>(
		this,
		&UMessageAgent::MessageAgent_HttpRequestComplete,
		requestURL,
		"POST",
		60.0F,
		Headers,
		JsonOutput
	);
}

void UMessageAgent::MessageAgent_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// [{"user":"eliza","text":"Loud and clear! My circuits are humming along, ready for whatever test you throw my way. If you need a debugging buddy or just someone to share the testing woes with, I'm here. How's everything looking on your end?","action":"NONE"}]
	if (bSucceeded) {
		TSharedPtr<FJsonValue> JsonValue;
		FString ResponseString = HttpResponse->GetContentAsString();
		UE_LOG(LogEliza, Display, TEXT("Message Agent response: %s"), *ResponseString);
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			if (JsonValue->Type == EJson::Array) {
				TSharedPtr<FJsonValue> MessageData = JsonValue->AsArray()[0];
				FString User = MessageData->AsObject()->GetStringField(TEXT("user"));
				FString Text = MessageData->AsObject()->GetStringField(TEXT("text"));
				FString Action = MessageData->AsObject()->GetStringField(TEXT("action"));

				OnMessageAgentCompleted.Broadcast(true, User, Text, Action);
				return;
			}
			else {
				//an error happened @TODO handle it
			}
		}
	}

	OnMessageAgentCompleted.Broadcast(false, FString(), FString(), FString());
	return;
}
