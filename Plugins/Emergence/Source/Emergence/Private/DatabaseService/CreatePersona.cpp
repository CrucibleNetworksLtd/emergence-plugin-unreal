// Fill out your copyright notice in the Description page of Project Settings.


#include "DatabaseService/CreatePersona.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UCreatePersona* UCreatePersona::CreatePersona(const UObject* WorldContextObject, FEmergencePersona Persona)
{
	UCreatePersona* BlueprintNode = NewObject<UCreatePersona>();
	BlueprintNode->TempPersona = Persona;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UCreatePersona::Activate()
{
	auto Emergence = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	FString AccessToken = Emergence->GetCurrentAccessToken();

	FString PersonaJsonString;
	FJsonObjectConverter::UStructToJsonObjectString<FEmergencePersona>(this->TempPersona, PersonaJsonString);
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	Headers.Add(TPair<FString, FString>{"Authorization", AccessToken});
	UHttpHelperLibrary::ExecuteHttpRequest<UCreatePersona>(
		this, 
		&UCreatePersona::CreatePersona_HttpRequestComplete, 
		"https://7h2e4n5z6i.execute-api.us-east-1.amazonaws.com/staging/persona",
		"POST",
		60.0F,
		Headers,
		PersonaJsonString);
	UE_LOG(LogTemp, Display, TEXT("CreatePersona request started with JSON, calling CreatePersona_HttpRequestComplete on request completed. Json sent as part of the request: "));
	UE_LOG(LogTemp, Display, TEXT("%s"), *PersonaJsonString);
}

void UCreatePersona::CreatePersona_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersona ResponceStruct = FEmergencePersona(*HttpResponse->GetContentAsString());
		OnCreatePersonaCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
		return;
	}
	OnCreatePersonaCompleted.Broadcast(FEmergencePersona(), StatusCode);
}
