// Copyright Crucible Networks Ltd 2025. All Rights Reserved.


#include "GetAgents.h"
#include "HttpService/ElizaHttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UGetAgents* UGetAgents::GetAgents()
{
	UGetAgents* BlueprintNode = NewObject<UGetAgents>();
	return BlueprintNode;
}

void UGetAgents::Activate()
{
	FString requestURL = UElizaHttpHelperLibrary::GetElizaStarterUrl() + "/Agents";
	//TArray<TPair<FString, FString>> Headers;
	//Headers.Add(TPair<FString, FString>{"content-type", "application/json"});

	/*TSharedPtr<FJsonObject> BodyContentJsonObject = MakeShareable(new FJsonObject);
	BodyContentJsonObject->SetStringField("text", Message);
	FString JsonOutput;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonOutput);
	FJsonSerializer::Serialize(BodyContentJsonObject.ToSharedRef(), Writer);
	*/
	UElizaHttpHelperLibrary::ExecuteHttpRequest<UGetAgents>(
		this,
		&UGetAgents::GetAgents_HttpRequestComplete,
		requestURL/*,
		"GET",
		60.0F,
		Headers,
		JsonOutput*/
	);
}

void UGetAgents::GetAgents_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	// [{"user":"eliza","text":"Loud and clear! My circuits are humming along, ready for whatever test you throw my way. If you need a debugging buddy or just someone to share the testing woes with, I'm here. How's everything looking on your end?","action":"NONE"}]
	if (bSucceeded) {
		TSharedPtr<FJsonValue> JsonValue;
		FString ResponseString = HttpResponse->GetContentAsString();
		UE_LOG(LogEliza, Display, TEXT("Get Agents response: %s"), *ResponseString);
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
		if (FJsonSerializer::Deserialize(Reader, JsonValue)) {
			TArray<FAgentInfo> Agents;
			auto AgentsArray = JsonValue->AsObject()->GetArrayField("agents");
			for (int i = 0; i < AgentsArray.Num(); i++) {
				auto Agent = AgentsArray[i]->AsObject();
				Agents.Add(FAgentInfo(Agent->GetStringField("id"), Agent->GetStringField("name")));
			}
			OnGetAgentsCompleted.Broadcast(true, Agents);

			return;
		}
	}

	OnGetAgentsCompleted.Broadcast(false, TArray<FAgentInfo>());
	return;
}
