// Fill out your copyright notice in the Description page of Project Settings.


#include "Futurepass/SendARTMAsync.h"
#include "WalletService/RequestToSign.h"
#include "UObject/UObjectGlobals.h"
#include "EmergenceSingleton.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"

USendARTMAsync* USendARTMAsync::SendARTMAsync(UObject* _WorldContextObject, FString Message, TArray<FEmergenceFutureverseARTMOperation> ARTMOperations) {
	USendARTMAsync* BlueprintNode = NewObject<USendARTMAsync>();
	BlueprintNode->_MessageToUser = Message;
	BlueprintNode->_ARTMOperations = ARTMOperations;
	BlueprintNode->WorldContextObject = _WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(_WorldContextObject);
	return BlueprintNode;
}

void USendARTMAsync::Activate()
{
	if (WorldContextObject && !UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->HasCachedAddress()) {
		UE_LOG(LogEmergence, Error, TEXT("Tried to get the futurepass user address but it has never been set. To use this function you must have the user login with futurepass."));
		return;
	}
	FString EoAAddress = UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCachedChecksummedAddress();

	//THIS MUST BE EOA / ETH CHECKSUMMED
	GetNonceRequest = UHttpHelperLibrary::ExecuteHttpRequest<USendARTMAsync>(
		this,
		nullptr,
		UHttpHelperLibrary::GetFutureverseAssetRegistryAPIURL(),
		"POST",
		300.0F, //give the user lots of time to mess around setting high gas fees
		TArray<TPair<FString,FString>>(),
		FString(), false);
	GetNonceRequest->SetHeader("content-type", "application/json");
	GetNonceRequest->SetContentAsString(R"({"query":"query GetNonce($input: NonceInput!) {\n  getNonceForChainAddress(input: $input)\n}","variables":{"input":{"chainAddress":")" + EoAAddress + "\"}}}");
	GetNonceRequest->OnProcessRequestComplete().BindLambda([&, EoAAddress](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
		//when the request finishes
		EErrorCode StatusCode;
		FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(res, bSucceeded, StatusCode);
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetNonce_HttpRequestComplete: %s"), *res->GetContentAsString());
		if (StatusCode == EErrorCode::EmergenceOk && !JsonObject.HasField("errors")) {
			int Nonce = JsonObject.GetObjectField("data")->GetIntegerField("getNonceForChainAddress");
			ConstructedMessage = UARTMBuilderLibrary::GenerateARTM(_MessageToUser, _ARTMOperations, EoAAddress, FString::FromInt(Nonce)).ReplaceCharWithEscapedChar();
			URequestToSign* URequestToSign = URequestToSign::RequestToSign(WorldContextObject, ConstructedMessage);
			UE_LOG(LogEmergenceHttp, Display, TEXT("Sending the following message to Request to Sign: %s"), *ConstructedMessage);
			URequestToSign->OnRequestToSignCompleted.AddDynamic(this, &USendARTMAsync::OnRequestToSignCompleted);
			RequestToSignRequest = URequestToSign->Request;
			URequestToSign->Activate();
			return;
		}
		else {
			//handle fail to parse errors
			OnSendARTMCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
			UE_LOG(LogEmergence, Error, TEXT("Send Mutation Request failed"));
			return;
		}
	});
	GetNonceRequest->ProcessRequest();
	
}

void USendARTMAsync::Cancel()
{
	if (GetNonceRequest) {
		GetNonceRequest->OnProcessRequestComplete().Unbind();
		GetNonceRequest->CancelRequest();
	}
	if (RequestToSignRequest) {
		RequestToSignRequest->OnProcessRequestComplete().Unbind();
		RequestToSignRequest->CancelRequest();
	}
	if (SendMutationRequest) {
		SendMutationRequest->OnProcessRequestComplete().Unbind();
		SendMutationRequest->CancelRequest();
	}
}

bool USendARTMAsync::IsActive() const
{
	return GetNonceRequest->GetStatus() == EHttpRequestStatus::Processing
		|| RequestToSignRequest->GetStatus() == EHttpRequestStatus::Processing
		|| SendMutationRequest->GetStatus() == EHttpRequestStatus::Processing;
}

void USendARTMAsync::OnRequestToSignCompleted(FString SignedMessage, EErrorCode StatusCode)
{
	UE_LOG(LogTemp, Display, TEXT("OnRequestToSignCompleted: %s"), *SignedMessage);

	if (StatusCode != EErrorCode::EmergenceOk) {
		//handle fail to parse errors
		OnSendARTMCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		UE_LOG(LogEmergence, Error, TEXT("Request To Sign Request failed"));
		return;
	}

	SendMutationRequest = UHttpHelperLibrary::ExecuteHttpRequest<USendARTMAsync>(
		this,
		nullptr,
		UHttpHelperLibrary::GetFutureverseAssetRegistryAPIURL(),
		"POST",
		60.0F,
		TArray<TPair<FString, FString>>(),
		FString(), false);
	SendMutationRequest->SetHeader("content-type", "application/json");
	FString JSONString = R"({"query":"mutation SubmitTransaction($input: SubmitTransactionInput!) {\n  submitTransaction(input: $input) {\n    transactionHash\n  }\n}","variables":{"input":{"transaction":")" + ConstructedMessage + R"(","signature":")" + SignedMessage + "\"}}}";

	SendMutationRequest->SetContentAsString(JSONString);
	SendMutationRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
		//when the request finishes
		EErrorCode StatusCode;
		FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(res, bSucceeded, StatusCode);
		UE_LOG(LogEmergenceHttp, Display, TEXT("SendMutationRequest_HttpRequestComplete: %s"), *res->GetContentAsString());
		if (StatusCode == EErrorCode::EmergenceOk && !JsonObject.HasField("errors")) {
			FString TransactionHash = JsonObject.GetObjectField("data")->GetObjectField("submitTransaction")->GetStringField("transactionHash");
			OnSendARTMCompleted.Broadcast(TransactionHash, EErrorCode::EmergenceOk);
		}
		else {
			//handle fail to parse errors
			OnSendARTMCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
			UE_LOG(LogEmergence, Error, TEXT("Send Mutation Request failed"));
			return;
		}
	});
	SendMutationRequest->ProcessRequest();
}
