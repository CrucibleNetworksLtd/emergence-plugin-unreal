// Fill out your copyright notice in the Description page of Project Settings.

#include "DatabaseService/GetCurrentPersona.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"

UGetCurrentPersona* UGetCurrentPersona::GetCurrentPersona()
{
	UGetCurrentPersona* BlueprintNode = NewObject<UGetCurrentPersona>();
	return BlueprintNode;
}

void UGetCurrentPersona::Activate()
{
	UHttpHelperLibrary::ExecuteHttpRequest<UGetCurrentPersona>(this, &UGetCurrentPersona::GetCurrentPersona_HttpRequestComplete, "https://7h2e4n5z6i.execute-api.us-east-1.amazonaws.com/staging/persona");
	UE_LOG(LogTemp, Display, TEXT("GetCurrentPersona request started, calling GetCurrentPersona_HttpRequestComplete on request completed"));
}

void UGetCurrentPersona::GetCurrentPersona_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersonaResponse ResponceStruct = FEmergencePersonaResponse(*HttpResponse->GetContentAsString());
		OnGetCurrentPersonaCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
		return;
	}
	OnGetCurrentPersonaCompleted.Broadcast(FEmergencePersonaResponse(), StatusCode);
}
