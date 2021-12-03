// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "DatabaseService/GetUserUnlockedAvatars.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UGetUserUnlockedAvatars* UGetUserUnlockedAvatars::GetUserUnlockedAvatars(const UObject* WorldContextObject)
{
	UGetUserUnlockedAvatars* BlueprintNode = NewObject<UGetUserUnlockedAvatars>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UGetUserUnlockedAvatars::Activate()
{
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken()});

	UHttpHelperLibrary::ExecuteHttpRequest<UGetUserUnlockedAvatars>(
		this, 
		&UGetUserUnlockedAvatars::GetUserUnlockedAvatars_HttpRequestComplete, 
		UHttpHelperLibrary::DatabaseAPIPrivate + "userUnlockedAvatars",
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogTemp, Display, TEXT("GetUserUnlockedAvatars request started, calling GetUserUnlockedAvatars_HttpRequestComplete on request completed"));
	UE_LOG(LogTemp, Display, TEXT("%s"), *UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken());
}

void UGetUserUnlockedAvatars::GetUserUnlockedAvatars_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	UE_LOG(LogTemp, Display, TEXT("%s"), *HttpResponse->GetContentAsString());
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergenceAvatarListResponse ResponceStruct = FEmergenceAvatarListResponse(*HttpResponse->GetContentAsString());
		OnGetUserUnlockedAvatarsCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
		return;
	}
	OnGetUserUnlockedAvatarsCompleted.Broadcast(FEmergenceAvatarListResponse(), StatusCode);
}
