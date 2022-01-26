// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/LoadAccount.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

ULoadAccount* ULoadAccount::LoadAccount(const UObject* WorldContextObject, const FString &Name, const FString &Password, const FString &Path, const FString &NodeURL)
{
	ULoadAccount* BlueprintNode = NewObject<ULoadAccount>();
	BlueprintNode->Name = Name;
	BlueprintNode->Password = Password;
	BlueprintNode->Path = Path;
	BlueprintNode->NodeURL = NodeURL;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void ULoadAccount::Activate()
{
	auto Emergence = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	FString AccessToken = Emergence->GetCurrentAccessToken();

	TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	Json->SetStringField("name", this->Name);
	Json->SetStringField("password", this->Password);
	Json->SetStringField("path", this->Path);
	Json->SetStringField("nodeURL", this->NodeURL);

	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	Headers.Add(TPair<FString, FString>{"Authorization", AccessToken});
	bool success = UHttpHelperLibrary::ExecuteHttpRequest<ULoadAccount>(
		this,
		&ULoadAccount::LoadAccount_HttpRequestComplete,
		UHttpHelperLibrary::APIBase + "loadAccount",
		"POST",
		60.0F,
		Headers,
		OutputString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), success ? TEXT("True") : TEXT("False"));
	UE_LOG(LogEmergenceHttp, Display, TEXT("LoadAccount request started with JSON, calling LoadAccount_HttpRequestComplete on request completed. Json sent as part of the request: "));
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *OutputString);
}

void ULoadAccount::LoadAccount_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("LoadAccount_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnLoadAccountCompleted.Broadcast(HttpResponse->GetContentAsString(), EErrorCode::EmergenceOk);
	}
	else {
		OnLoadAccountCompleted.Broadcast(FString(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("LoadAccount", StatusCode);
	}
}
