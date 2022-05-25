// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "AvatarService/AvatarByOwner.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UAvatarByOwner* UAvatarByOwner::AvatarByOwner(const UObject* WorldContextObject, const FString& Address)
{
	UAvatarByOwner* BlueprintNode = NewObject<UAvatarByOwner>();
	BlueprintNode->Address = FString(Address);
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UAvatarByOwner::Activate()
{
	FString requestURL = "http://localhost:14391/AvatarSystem/byOwner?address=" + Address;
	TArray<TPair<FString, FString>> Headers;
	//Headers.Add(TPair<FString, FString>{"Authorization", UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken()});

	UHttpHelperLibrary::ExecuteHttpRequest<UAvatarByOwner>(
		this,
		&UAvatarByOwner::AvatarByOwner_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("AvatarByOwner request started, calling AvatarByOwner_HttpRequestComplete on request completed"));
}

void UAvatarByOwner::AvatarByOwner_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	StatusCode = EErrorCode::EmergenceOk; //FORCE IT, ONLY FOR TESTING
	if (StatusCode == EErrorCode::EmergenceOk) {
		TArray<FEmergenceAvatarResult> Results;
		FJsonObjectConverter::JsonArrayStringToUStruct<FEmergenceAvatarResult>(HttpResponse->GetContentAsString(), &Results, 0, 0);
		OnAvatarByOwnerCompleted.Broadcast(Results, EErrorCode::EmergenceOk);
		return;
	}

	OnAvatarByOwnerCompleted.Broadcast(TArray<FEmergenceAvatarResult>(), StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("AvatarByOwner", StatusCode);
}
