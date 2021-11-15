// Fill out your copyright notice in the Description page of Project Settings.


#include "DatabaseService/GetPersonaByID.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"

UGetPersonaByID* UGetPersonaByID::GetPersonaByID(const FString& personaID)
{
	UGetPersonaByID* BlueprintNode = NewObject<UGetPersonaByID>();
	BlueprintNode->PersonaID = FString(personaID);
	return BlueprintNode;
}

void UGetPersonaByID::Activate()
{
	FString requestURL = "https://7h2e4n5z6i.execute-api.us-east-1.amazonaws.com/staging/persona/" + PersonaID;
	UHttpHelperLibrary::ExecuteHttpRequest<UGetPersonaByID>(this, &UGetPersonaByID::GetPersonaByID_HttpRequestComplete, requestURL);
	UE_LOG(LogTemp, Display, TEXT("GetPersonaByID request started, calling GetPersonaByID_HttpRequestComplete on request completed"));
}

void UGetPersonaByID::GetPersonaByID_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersona ResponceStruct = FEmergencePersona(*HttpResponse->GetContentAsString());
		OnGetPersonaByIDCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
		return;
	}
	OnGetPersonaByIDCompleted.Broadcast(FEmergencePersona(), StatusCode);
}
