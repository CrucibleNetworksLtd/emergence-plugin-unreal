// Copyright Crucible Networks Ltd 2025. All Rights Reserved.


#include "SetAgentCharacter.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

USetAgentCharacter* USetAgentCharacter::SetAgentCharacter(FAgentDetails AgentDetails)
{
	USetAgentCharacter* BlueprintNode = NewObject<USetAgentCharacter>();
	BlueprintNode->AgentDetails = AgentDetails;
	return BlueprintNode;
}

void USetAgentCharacter::Activate()
{
	FString requestURL = UElizaHttpHelperLibrary::GetElizaStarterUrl() + "/Agents/" + AgentDetails.id + "/set";
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	FString JsonOutput;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonOutput);
	FJsonSerializer::Serialize(AgentDetails.FAgentDetailsToJson()->GetObjectField("character").ToSharedRef(), Writer);
	
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
		/*TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {

			FAgentDetails AgentDetails = FAgentDetails(HttpResponse->GetContentAsString());
			OnSetAgentCharacterCompleted.Broadcast(true, AgentDetails);

			return;
		}*/
	}

	OnSetAgentCharacterCompleted.Broadcast(false, FAgentDetails());
	return;
}
