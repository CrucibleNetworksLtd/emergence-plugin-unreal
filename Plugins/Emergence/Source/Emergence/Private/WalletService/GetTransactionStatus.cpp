// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/GetTransactionStatus.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "EmergenceChainObject.h"
#include "WalletService/GetBlockNumber.h"

UGetTransactionStatus* UGetTransactionStatus::GetTransactionStatus(UObject* WorldContextObject, FString TransactionHash, UEmergenceChain* Blockchain)
{
	UGetTransactionStatus* BlueprintNode = NewObject<UGetTransactionStatus>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->TransactionHash = TransactionHash;
	BlueprintNode->Blockchain = Blockchain;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UGetTransactionStatus::Activate()
{
	if (Blockchain) {

		UGetBlockNumber* GetBlockNumberRequest = UGetBlockNumber::GetBlockNumber(WorldContextObject, this->Blockchain);
		GetBlockNumberRequest->OnGetBlockNumberCompleted.AddDynamic(this, &UGetTransactionStatus::GetBlockNumberCompleted);
		GetBlockNumberRequest->Activate();		
	}
	else {
		UE_LOG(LogEmergenceHttp, Error, TEXT("GetTransactionStatus' blockchain input was null."));
		OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceClientFailed);
	}
}

void UGetTransactionStatus::GetTransactionStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetTransactionStatus_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {	
		FString TransactionAsJSONString;
		UE_LOG(LogTemp, Display, TEXT("%s"), *HttpResponse->GetContentAsString());
		TSharedPtr<FJsonObject> Result;
		if (JsonObject.HasTypedField<EJson::Null>("result")) {
			OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceOk);
		}
		else if (Result = JsonObject.GetObjectField("result")) {
			TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&TransactionAsJSONString);
			FJsonSerializer::Serialize(Result.ToSharedRef(), Writer);
			UE_LOG(LogTemp, Display, TEXT("%s"), *TransactionAsJSONString);
			
			FEmergenceTransaction Transaction;
			Transaction.To = Result->GetStringField("to");
			Transaction.Type = FParse::HexNumber(*Result->GetStringField("type"));
			Transaction.Status = Result->GetStringField("status") == "0x1" ? true : false;
			Result->TryGetStringField("contractAddress", Transaction.ContractAddress);
			Transaction.CumulativeGasUsed = FParse::HexNumber(*Result->GetStringField("cumulativeGasUsed"));
			Transaction.GasUsed = FParse::HexNumber(*Result->GetStringField("gasUsed"));
			Transaction.From = Result->GetStringField("from");
			Transaction.BlockNumber = FParse::HexNumber(*Result->GetStringField("blockNumber"));
			Transaction.BlockHash = Result->GetStringField("blockHash");
			Transaction.TransactionIndex = FParse::HexNumber(*Result->GetStringField("transactionIndex"));
			Result->TryGetStringField("hash", Transaction.TransactionHash);
			Transaction.Confirmations = this->BlockNumber - Transaction.BlockNumber;
			OnGetTransactionStatusCompleted.Broadcast(Transaction, EErrorCode::EmergenceOk);

		}
		else{
			OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceClientJsonParseFailed);
			UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetTransactionStatus", EErrorCode::EmergenceClientJsonParseFailed);
		}
	}
	else {
		OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetTransactionStatus", StatusCode);
	}
	SetReadyToDestroy();
}

void UGetTransactionStatus::GetBlockNumberCompleted(int64 _BlockNumber, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		this->BlockNumber = _BlockNumber;

		TArray<TPair<FString, FString>> Headers;
		Headers.Add(TPair<FString, FString>("Content-Type", "application/json"));
		UHttpHelperLibrary::ExecuteHttpRequest<UGetTransactionStatus>(
			this,
			&UGetTransactionStatus::GetTransactionStatus_HttpRequestComplete,
			this->Blockchain->NodeURL,
			"POST",
			60.0F,
			Headers,
			R"({"jsonrpc":"2.0", "method":"eth_getTransactionReceipt", "params":[")" + this->TransactionHash + R"("], "id":1 })");
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetTransactionStatus request started with JSON, calling GetTransactionStatus_HttpRequestComplete on request completed."));
	}
	else {
		OnGetTransactionStatusCompleted.Broadcast(FEmergenceTransaction(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("GetTransactionStatus", StatusCode);
		SetReadyToDestroy();
	}
}