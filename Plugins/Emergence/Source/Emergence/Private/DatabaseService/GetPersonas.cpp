// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "DatabaseService/GetPersonas.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UGetPersonas* UGetPersonas::GetPersonas(const UObject* WorldContextObject)
{
	UGetPersonas* BlueprintNode = NewObject<UGetPersonas>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UGetPersonas::Activate()
{
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken()});

	UHttpHelperLibrary::ExecuteHttpRequest<UGetPersonas>(
		this,
		&UGetPersonas::GetPersonas_HttpRequestComplete,
		UHttpHelperLibrary::DatabaseAPIPrivate + "personas",
		"GET",
		60.0F,
		Headers
		);
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
