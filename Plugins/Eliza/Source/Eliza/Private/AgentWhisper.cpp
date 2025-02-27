// Copyright Crucible Networks Ltd 2025. All Rights Reserved.


#include "AgentWhisper.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UAgentWhisper* UAgentWhisper::AgentWhisper(FString _AgentId, FString _Message, UElizaInstance* _ElizaInstance)
{
	UAgentWhisper* BlueprintNode = NewObject<UAgentWhisper>();
	BlueprintNode->AgentId = _AgentId;
	BlueprintNode->Message = _Message;
	BlueprintNode->ElizaInstanceOverride = _ElizaInstance;
	return BlueprintNode;
}

void UAgentWhisper::Activate()
{
	FString requestURL;

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	if (ElizaInstanceOverride) {
		if (ElizaInstanceOverride->ElizaInstance.APIType == EElizaAPIType::GenericEliza) {
			requestURL = ElizaInstanceOverride->ElizaInstance.LocationURL + "/" + AgentId + "/Whisper";
		}
		if (ElizaInstanceOverride->ElizaInstance.APIType == EElizaAPIType::Fleek) {
			requestURL = "https://api.fleek.xyz/api/v1/ai-agents/" + ElizaInstanceOverride->ElizaInstance.FleekAgentId + "/api/" + AgentId + "/Whisper";
			Headers.Add(TPair<FString, FString>{"x-api-key", "" + ElizaInstanceOverride->ElizaInstance.FleekAPIKey});
		}
	}
	else {
		requestURL = UElizaHttpHelperLibrary::GetElizaStarterUrl() + "/" + AgentId + "/Whisper";
	}
	

	TSharedPtr<FJsonObject> BodyContentJsonObject = MakeShareable(new FJsonObject);
	BodyContentJsonObject->SetStringField(TEXT("text"), Message);
	FString JsonOutput;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonOutput);
	FJsonSerializer::Serialize(BodyContentJsonObject.ToSharedRef(), Writer);

	UElizaHttpHelperLibrary::ExecuteHttpRequest<UAgentWhisper>(
		this,
		&UAgentWhisper::AgentWhisper_HttpRequestComplete,
		requestURL,
		"POST",
		60.0F,
		Headers,
		JsonOutput
	);
}

void UAgentWhisper::AgentWhisper_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// [{"user":"eliza","text":"Loud and clear! My circuits are humming along, ready for whatever test you throw my way. If you need a debugging buddy or just someone to share the testing woes with, I'm here. How's everything looking on your end?","action":"NONE"}]
	if (bSucceeded) {
		TSharedPtr<FJsonValue> JsonValue;
		FString ResponseString = HttpResponse->GetContentAsString();
		UE_LOG(LogEliza, Display, TEXT("Message Agent response: %s"), *ResponseString);
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			if (JsonValue->AsObject()->HasField("error")) {
				FString ErrorDetails = JsonValue->AsObject()->GetStringField("details");
				UE_LOG(LogEliza, Error, TEXT("Eliza error: %s"), *HttpResponse->GetContentAsString());
				if (ErrorDetails.Contains("api")) {
					UE_LOG(LogEliza, Error, TEXT("Error doing TTS. Your Eliza instance is missing its ElevenLabs API key."));
				}
			}


			//OnAgentWhisperCompleted.Broadcast(true, User, Text, Action);
			return;
		}
	}

	OnAgentWhisperCompleted.Broadcast(false, FString(), FString(), FString());
	return;
}
