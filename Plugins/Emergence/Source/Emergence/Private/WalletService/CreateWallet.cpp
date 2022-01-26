// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "WalletService/CreateWallet.h"
#include "DatabaseService/SetActivePersona.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UCreateWallet *UCreateWallet::CreateWallet(const UObject *WorldContextObject, const FString &Path, const FString &Password)
{
	UCreateWallet *BlueprintNode = NewObject<UCreateWallet>();
	BlueprintNode->Path = Path;
	BlueprintNode->Password = Password;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UCreateWallet::Activate()
{
	auto Emergence = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	FString AccessToken = Emergence->GetCurrentAccessToken();

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", AccessToken});
	UHttpHelperLibrary::ExecuteHttpRequest<UCreateWallet>(
		this,
		&UCreateWallet::CreateWallet_HttpRequestComplete,
		UHttpHelperLibrary::APIBase + "createWallet?path=" + Path + "&password=" + Password,
		"POST",
		60.0F,
		Headers);
	UE_LOG(LogEmergenceHttp, Display, TEXT("CreateWallet request started, calling CreateWallet_HttpRequestComplete on request completed"));
}

void UCreateWallet::CreateWallet_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk)
	{
		OnCreateWalletCompleted.Broadcast(FString(), EErrorCode::EmergenceOk);
		return;
	}
	OnCreateWalletCompleted.Broadcast(FString(), StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("CreateWallet", StatusCode);
}
