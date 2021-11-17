// Fill out your copyright notice in the Description page of Project Settings.


#include "DatabaseService/DeletePersonaByID.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"

UDeletePersonaByID* UDeletePersonaByID::DeletePersonaByID(const FString& PersonaID)
{
	UDeletePersonaByID* BlueprintNode = NewObject<UDeletePersonaByID>();
	BlueprintNode->PersonaID = FString(PersonaID);
	return BlueprintNode;
}

void UDeletePersonaByID::Activate()
{
	FString requestURL = UHttpHelperLibrary::DatabaseAPIPrivate + "persona/" + PersonaID;
	UHttpHelperLibrary::ExecuteHttpRequest<UDeletePersonaByID>(this, &UDeletePersonaByID::DeletePersonaByID_HttpRequestComplete, requestURL, "DELETE");
	UE_LOG(LogTemp, Display, TEXT("DeletePersonaByID request started, calling DeletePersonaByID_HttpRequestComplete on request completed"));
}

void UDeletePersonaByID::DeletePersonaByID_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergencePersona ResponceStruct = FEmergencePersona(*HttpResponse->GetContentAsString());
		OnDeletePersonaByIDCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
		return;
	}
	OnDeletePersonaByIDCompleted.Broadcast(FEmergencePersona(), StatusCode);
}
