// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/GetBalance.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UGetBalance* UGetBalance::GetBalance(const UObject* WorldContextObject, FString Address)
{
	UGetBalance* BlueprintNode = NewObject<UGetBalance>();
	BlueprintNode->Address = Address;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UGetBalance::Activate()
{
	const FString defaultNodeURL = "https://polygon-mainnet.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134";
	FString NodeURL;
	if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("NodeURL"), NodeURL, GGameIni) && NodeURL != "") //if we can get the string from the config and successfully parse it
	{
		UE_LOG(LogTemp, Warning, TEXT("NodeURL override: (%s)."), *NodeURL);
	}
	else {
		NodeURL = defaultNodeURL;
		UE_LOG(LogTemp, Warning, TEXT("Using default NODEURL (%s)."), *NodeURL);
	}

	bool success = UHttpHelperLibrary::ExecuteHttpRequest<UGetBalance>(
		this, 
		&UGetBalance::GetBalance_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "getbalance" + "?nodeUrl=" + NodeURL + "&address=" + this->Address);
	UE_LOG(LogTemp, Display, TEXT("%s"), success ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Display, TEXT("GetBalance request started with JSON, calling GetBalance_HttpRequestComplete on request completed. Json sent as part of the request: "));
}

void UGetBalance::GetBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{	
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString Balance;
		if (JsonObject.GetObjectField("message")->TryGetStringField("balance", Balance)) {
			OnGetBalanceCompleted.Broadcast(Balance, StatusCode);
		}
		else {
			OnGetBalanceCompleted.Broadcast(FString(), EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnGetBalanceCompleted.Broadcast(FString(), StatusCode);
}
