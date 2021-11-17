// Fill out your copyright notice in the Description page of Project Settings.


#include "DatabaseService/GetPersonas.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"

UGetPersonas* UGetPersonas::GetPersonas()
{
	UGetPersonas* BlueprintNode = NewObject<UGetPersonas>();
	return BlueprintNode;
}

void UGetPersonas::Activate()
{
	UHttpHelperLibrary::ExecuteHttpRequest<UGetPersonas>(this, &UGetPersonas::GetPersonas_HttpRequestComplete, UHttpHelperLibrary::DatabaseAPIPublic + "personas");
	UE_LOG(LogTemp, Display, TEXT("GetPersonas request started, calling GetPersonas_HttpRequestComplete on request completed"));
}

void UGetPersonas::GetPersonas_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersonaListResponse ResponceStruct = FEmergencePersonaListResponse(*HttpResponse->GetContentAsString());
		OnGetPersonasCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
		return;
	}
	OnGetPersonasCompleted.Broadcast(FEmergencePersonaListResponse(), StatusCode);
}
