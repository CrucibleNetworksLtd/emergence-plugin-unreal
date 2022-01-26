// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "WalletService/CreateKeyStore.h"
#include "DatabaseService/SetActivePersona.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UCreateKeyStore *UCreateKeyStore::CreateKeyStore(
	const UObject *WorldContextObject,
	const FString &PrivateKey,
	const FString &Password,
	const FString &PublicKey,
	const FString &Path)
{
	UCreateKeyStore *BlueprintNode = NewObject<UCreateKeyStore>();
	BlueprintNode->PrivateKey = PrivateKey;
	BlueprintNode->Password = Password;
	BlueprintNode->PublicKey = PublicKey;
	BlueprintNode->Path = Path;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UCreateKeyStore::Activate()
{
	auto Emergence = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	FString AccessToken = Emergence->GetCurrentAccessToken();

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Authorization", AccessToken});
	UHttpHelperLibrary::ExecuteHttpRequest<UCreateKeyStore>(
		this,
		&UCreateKeyStore::CreateKeyStore_HttpRequestComplete,
		UHttpHelperLibrary::APIBase + "createKeyStore" + "?privateKey=" + PrivateKey + "&password=" + Password + "&publicKey=" + PublicKey + "&path=" + Path,
		"POST",
		60.0F,
		Headers);
	UE_LOG(LogEmergenceHttp, Display, TEXT("CreateKeyStore request started, calling CreateKeyStore_HttpRequestComplete on request completed"));
}

void UCreateKeyStore::CreateKeyStore_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk)
	{
		OnCreateKeyStoreCompleted.Broadcast(FString(), EErrorCode::EmergenceOk);
		return;
	}
	OnCreateKeyStoreCompleted.Broadcast(FString(), StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("CreateKeyStore", StatusCode);
}
