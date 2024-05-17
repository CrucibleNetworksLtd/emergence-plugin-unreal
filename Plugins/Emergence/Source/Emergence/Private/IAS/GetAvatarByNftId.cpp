// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "IAS/GetAvatarByNftId.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UGetAvatarByNftId* UGetAvatarByNftId::GetAvatarByNftId(UObject* WorldContextObject, const FString& AvatarIdString)
{
	UGetAvatarByNftId* BlueprintNode = NewObject<UGetAvatarByNftId>();
	BlueprintNode->AvatarIdString = FString(AvatarIdString);
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetAvatarByNftId::Activate()
{
	FString requestURL = "http://interoperableassetsystem-env.eba-xcksw4pw.us-east-1.elasticbeanstalk.com/Avatar/GetByNFTId?nftId=" + AvatarIdString;
	TArray<TPair<FString, FString>> Headers;
	//Headers.Add(TPair<FString, FString>{"Host", UHttpHelperLibrary::GetAvatarServiceHostURL()});

	Request = UHttpHelperLibrary::ExecuteHttpRequest<UGetAvatarByNftId>(
		this,
		&UGetAvatarByNftId::GetAvatarByNftId_HttpRequestComplete,
		requestURL,
		"GET",
		60.0F,
		Headers
		);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetAvatarByNftId request started, calling GetAvatarByNftId_HttpRequestComplete on request completed"));
}

void UGetAvatarByNftId::GetAvatarByNftId_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogTemp, Display, TEXT("%s"), *HttpRequest->GetURL());
	UE_LOG(LogTemp, Display, TEXT("%s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnGetAvatarByNftIdCompleted.Broadcast(FEmergenceInteroperableAsset(HttpResponse->GetContentAsString()), StatusCode);
	}
	else {
		OnGetAvatarByNftIdCompleted.Broadcast(FEmergenceInteroperableAsset(), StatusCode);
	}
	SetReadyToDestroy();
}