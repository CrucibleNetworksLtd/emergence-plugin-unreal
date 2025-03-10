// Copyright Crucible Networks Ltd 2025. All Rights Reserved.


#include "Virtuals/EditVirtualsAgent.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UEditVirtualsAgent* UEditVirtualsAgent::EditVirtualsAgent(FString _EntityId, FString _Goal, FString _Description)
{
	UEditVirtualsAgent* BlueprintNode = NewObject<UEditVirtualsAgent>();
	BlueprintNode->EntityId = _EntityId;
	BlueprintNode->Goal = _Goal;
	BlueprintNode->Description = _Description;
	return BlueprintNode;
}

void UEditVirtualsAgent::Activate()
{
	FString requestURL = "https://blueprint-serve-8305v2t4v-narraio.vercel.app/virtual-agent/edit";

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	UElizaHttpHelperLibrary::ExecuteHttpRequest<UEditVirtualsAgent>(
		this,
		&UEditVirtualsAgent::EditVirtualsAgent_HttpRequestComplete,
		requestURL,
		"POST",
		60.0F,
		Headers,
		"{\"entityId\": \"" + EntityId + "\",\"virtuals_goal\": \"" + Goal + "\",\"virtuals_description\": \"" + Description + "\"}"
	);
}

void UEditVirtualsAgent::EditVirtualsAgent_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded) {
		TSharedPtr<FJsonValue> JsonValue;
		FString ResponseString = HttpResponse->GetContentAsString();
		UE_LOG(LogEliza, Display, TEXT("Edit Virtuals Agent response: %s"), *ResponseString);
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			auto Object = JsonValue->AsObject();
			if(Object->GetStringField(TEXT("success")) == "true"){
				OnEditVirtualsAgentCompleted.Broadcast(true);
				return;
			}
		}
	}

	OnEditVirtualsAgentCompleted.Broadcast(false);
	return;
}
