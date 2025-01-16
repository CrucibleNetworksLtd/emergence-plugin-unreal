// Copyright Crucible Networks Ltd 2025. All Rights Reserved.


#include "MessageAgent.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"


UMessageAgent* UMessageAgent::MessageAgent(FString _AgentId, FString _Message)
{
	UMessageAgent* BlueprintNode = NewObject<UMessageAgent>();
	BlueprintNode->AgentId = _AgentId;
	BlueprintNode->Message = _Message;
	return BlueprintNode;
}

void UMessageAgent::Activate()
{
	FString requestURL = UElizaHttpHelperLibrary::GetElizaStarterUrl() + AgentId + "/message";
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	TSharedRef<FJsonObject> BodyContentJsonObject;
	BodyContentJsonObject->SetStringField("text", Message);
	FString JsonOutput;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonOutput);
	FJsonSerializer::Serialize(BodyContentJsonObject, Writer);
	FString BodyContentString;

	UElizaHttpHelperLibrary::ExecuteHttpRequest<UMessageAgent>(
		this,
		&UMessageAgent::MessageAgent_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers,
		BodyContentString
	);
}

void UMessageAgent::MessageAgent_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// {"Data":[{"user":"eliza","text":"Testing, testing, 1-2-3! All systems operational, though my confidence intervals are a bit shaky today. Just making sure my response tensors are properly aligned. How's the audio coming through on your end?","action":"NONE"}]}
	if (bSucceeded) {
		TSharedPtr<FJsonValue> JsonValue;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			TSharedPtr<FJsonValue> MessageData = JsonValue->AsObject()->GetArrayField("Data")[0];
			FString User = MessageData->AsObject()->GetStringField("user");
			FString Text = MessageData->AsObject()->GetStringField("text");
			FString Action = MessageData->AsObject()->GetStringField("action");

			OnMessageAgentCompleted.Broadcast(true, User, Text, Action);
			return;
		}
	}

	OnMessageAgentCompleted.Broadcast(false, FString(), FString(), FString());
	return;
}
