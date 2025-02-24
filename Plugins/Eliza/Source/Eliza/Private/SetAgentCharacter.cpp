// Copyright Crucible Networks Ltd 2025. All Rights Reserved.


#include "SetAgentCharacter.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

USetAgentCharacter* USetAgentCharacter::SetAgentCharacter(FAgentDetailsCharacter _AgentCharacter, bool _CreateNew, UElizaInstance* _ElizaInstance)
{
	USetAgentCharacter* BlueprintNode = NewObject<USetAgentCharacter>();
	BlueprintNode->AgentCharacter = _AgentCharacter;
	BlueprintNode->ElizaInstanceOverride = _ElizaInstance;
	BlueprintNode->CreateNew = _CreateNew;
	return BlueprintNode;
}

void USetAgentCharacter::Activate()
{
	FString AgentID;
	
	if (CreateNew) {
		AgentID = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphensLower);
	}
	else {
		AgentID = AgentCharacter.id; //Get the ID from the given data struct
	}

	FString requestURL;

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	if (ElizaInstanceOverride) {
		if (ElizaInstanceOverride->ElizaInstance.APIType == EElizaAPIType::GenericEliza) {
			requestURL = ElizaInstanceOverride->ElizaInstance.LocationURL + "/Agents/" + AgentID + "/set";
		}
		if (ElizaInstanceOverride->ElizaInstance.APIType == EElizaAPIType::Fleek) {
			requestURL = "https://api.fleek.xyz/api/v1/ai-agents/" + ElizaInstanceOverride->ElizaInstance.FleekAgentId + "/api/Agents/" + AgentID + "/set";
			Headers.Add(TPair<FString, FString>{"x-api-key", "" + ElizaInstanceOverride->ElizaInstance.FleekAPIKey});
		}
	}
	else {
		requestURL = UElizaHttpHelperLibrary::GetElizaStarterUrl() + "/Agents/" + AgentID + "/set";
	}
	

	FString JsonOutput;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonOutput);
	auto CharacterJson = AgentCharacter.FAgentDetailsCharacterToJson().ToSharedRef();
	//CharacterJson->RemoveField("id");
	CharacterJson->SetArrayField("clients", TArray<TSharedPtr<FJsonValue>>());
	CharacterJson->SetArrayField("plugins", TArray<TSharedPtr<FJsonValue>>());
	CharacterJson->SetStringField("id", AgentID);
	FJsonSerializer::Serialize(CharacterJson, Writer);
	
	UE_LOG(LogTemp, Display, TEXT("%s"), *JsonOutput);

	UElizaHttpHelperLibrary::ExecuteHttpRequest<USetAgentCharacter>(
		this,
		&USetAgentCharacter::SetAgentCharacter_HttpRequestComplete,
		requestURL,
		"POST",
		60.0F,
		Headers,
		JsonOutput
	);
}

void USetAgentCharacter::SetAgentCharacter_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded) {
		TSharedPtr<FJsonValue> JsonValue;
		FString ResponseString = HttpResponse->GetContentAsString();
		UE_LOG(LogEliza, Display, TEXT("Set Agent Character response: %s"), *ResponseString);
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			OnSetAgentCharacterCompleted.Broadcast(true, JsonValue->AsObject()->GetStringField("id"));

			return;
		}
	}

	OnSetAgentCharacterCompleted.Broadcast(false, "");
	return;
}
