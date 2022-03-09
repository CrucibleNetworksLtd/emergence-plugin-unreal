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
	FString GameID;
	if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("GameID"), GameID, GGameIni) && GameID != "") //if we can get the string from the config and successfully parse it
	{
		UE_LOG(LogEmergenceHttp, Display, TEXT("Game ID set to: (%s)."), *GameID);
	}
	else
	{
		UE_LOG(LogEmergenceHttp, Error, TEXT("Could not get custom Game ID from plugin settings, using default-game-id"));
		GameID = TEXT("default-game-id");
	}

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken()});

	UHttpHelperLibrary::ExecuteHttpRequest<UGetUserUnlockedAvatars>(
		this, 
		&UGetUserUnlockedAvatars::GetUserUnlockedAvatars_HttpRequestComplete, 
		UHttpHelperLibrary::DatabaseAPIPrivate + "userUnlockedAvatars?id=" + GameID,
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetUserUnlockedAvatars request started, calling GetUserUnlockedAvatars_HttpRequestComplete on request completed"));
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCurrentAccessToken());
}

void UGetUserUnlockedAvatars::GetUserUnlockedAvatars_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *HttpResponse->GetContentAsString());
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FEmergenceAvatarListResponse ResponceStruct = FEmergenceAvatarListResponse(*HttpResponse->GetContentAsString());
		OnGetUserUnlockedAvatarsCompleted.Broadcast(ResponceStruct, EErrorCode::EmergenceOk);
		return;
	}
	OnGetUserUnlockedAvatarsCompleted.Broadcast(FEmergenceAvatarListResponse(), StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetUserUnlockedAvatars", StatusCode);
}
