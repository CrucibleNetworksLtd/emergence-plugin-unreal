// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/GetBlockNumber.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "EmergenceChainObject.h"

UGetBlockNumber* UGetBlockNumber::GetBlockNumber(UObject* WorldContextObject, UEmergenceChain* Blockchain)
{
	UGetBlockNumber* BlueprintNode = NewObject<UGetBlockNumber>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->Blockchain = Blockchain;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetBlockNumber::Activate()
{
	if (Blockchain) {
		TArray<TPair<FString, FString>> Headers;
		Headers.Add(TPair<FString, FString>("Content-Type", "application/json"));
		UHttpHelperLibrary::ExecuteHttpRequest<UGetBlockNumber>(
			this,
			&UGetBlockNumber::GetBlockNumber_HttpRequestComplete,
			this->Blockchain->NodeURL,
			"POST",
			60.0F,
			Headers,
			R"({"jsonrpc":"2.0", "method":"eth_blockNumber", "params":[] , "id":1})");
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetBlockNumber request started with JSON, calling GetBlockNumber_HttpRequestComplete on request completed."));
	}
	else {
		UE_LOG(LogEmergenceHttp, Error, TEXT("GetBlockNumber' blockchain input was null."));
		OnGetBlockNumberCompleted.Broadcast(-1, EErrorCode::EmergenceClientFailed);
	}
}

void UGetBlockNumber::GetBlockNumber_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString BlockNumberAsHex;
		if (JsonObject.TryGetStringField("result", BlockNumberAsHex)) {
			OnGetBlockNumberCompleted.Broadcast(FParse::HexNumber(*BlockNumberAsHex), EErrorCode::EmergenceOk);
		}
		else {
			OnGetBlockNumberCompleted.Broadcast(-1, StatusCode);
			UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetBlockNumber", StatusCode);
		}
	}
	else {
		OnGetBlockNumberCompleted.Broadcast(-1, StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetBlockNumber", StatusCode);
	}
	SetReadyToDestroy();
}
