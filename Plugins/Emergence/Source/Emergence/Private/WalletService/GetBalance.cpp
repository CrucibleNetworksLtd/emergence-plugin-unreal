// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/GetBalance.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "EmergenceChainObject.h"
#include "WalletService/BigIntFunctionLibrary.h"

UGetBalance* UGetBalance::GetBalance(UObject* WorldContextObject, FString Address, UEmergenceChain* Blockchain)
{
	UGetBalance* BlueprintNode = NewObject<UGetBalance>();
	BlueprintNode->Address = Address;
	BlueprintNode->Blockchain = Blockchain;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetBalance::Activate()
{
	if (Blockchain) {
		TArray<TPair<FString, FString>> Headers;
		Headers.Add(TPair<FString, FString>("Content-Type", "application/json"));
		UHttpHelperLibrary::ExecuteHttpRequest<UGetBalance>(
			this,
			&UGetBalance::GetBalance_HttpRequestComplete,
			this->Blockchain->NodeURL,
			"POST",
			60.0F,
			Headers,
			R"({"jsonrpc":"2.0", "method":"eth_getBalance", "params":[")" + this->Address + R"(", "latest"] , "id":1})");
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetBalance request started with JSON, calling GetBalance_HttpRequestComplete on request completed. Json sent as part of the request: "));
	}
	else {
		UE_LOG(LogEmergenceHttp, Error, TEXT("GetBalance's blockchain input was null."));
		OnGetBalanceCompleted.Broadcast(FString(), EErrorCode::EmergenceClientFailed);
	}
}

void UGetBalance::GetBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{	
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		UE_LOG(LogTemp, Display, TEXT("%s"), *HttpResponse->GetContentAsString());
		FString BlockNumberAsHex;
		if (JsonObject.TryGetStringField("result", BlockNumberAsHex)) {
			OnGetBalanceCompleted.Broadcast(UBigIntFunctionLibrary::HexToDec(*BlockNumberAsHex), EErrorCode::EmergenceOk);
		}
		else {
			OnGetBalanceCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		}
	}
	else {
		OnGetBalanceCompleted.Broadcast(FString(), StatusCode);
	}
	SetReadyToDestroy();
}
