// Fill out your copyright notice in the Description page of Project Settings.


#include "Futurepass/SendARTMAsync.h"
#include "WalletService/RequestToSign.h"
#include "UObject/UObjectGlobals.h"
#include "EmergenceSingleton.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Interfaces/IHttpResponse.h"

USendARTMAsync* USendARTMAsync::SendARTMAsync(UObject* _WorldContextObject, FString Message, TArray<FEmergenceFutureverseARTMOperation> ARTMOperations) {
	USendARTMAsync* BlueprintNode = NewObject<USendARTMAsync>();
	BlueprintNode->_Message = Message;
	BlueprintNode->_ARTMOperations = ARTMOperations;
	BlueprintNode->WorldContextObject = _WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(_WorldContextObject);
	return BlueprintNode;
}

void USendARTMAsync::Activate()
{
	if (WorldContextObject && !UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->FuturepassInfoIsSet) {
		UE_LOG(LogEmergence, Error, TEXT("Tried to get the futurepass user address but it has never been set. To use this function you must have the user login with futurepass."));
		return;
	}
	TArray<FString> Out;
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->FuturepassInfo.futurepass.ParseIntoArray(Out, TEXT(":"), true);
	FString FuturepassAddress = Out.Last();

	GetNonceRequest = UHttpHelperLibrary::ExecuteHttpRequest<USendARTMAsync>(
		this,
		nullptr,
		"https://6b20qa1273.execute-api.us-west-2.amazonaws.com/graphql",
		"POST",
		300.0F, //give the user lots of time to mess around setting high gas fees
		TArray<TPair<FString,FString>>(),
		FString(), false);
	GetNonceRequest->SetHeader("content-type", "application/json");
	GetNonceRequest->SetContentAsString(R"({"query":"query GetNonce($input: NonceInput!) {\n  getNonceForChainAddress(input: $input)\n}","variables":{"input":{"chainAddress":")" + FuturepassAddress + "\"}}}");
	GetNonceRequest->OnProcessRequestComplete().BindLambda([&, FuturepassAddress](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
		EErrorCode StatusCode;
		FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(res, bSucceeded, StatusCode);
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetNonce_HttpRequestComplete: %s"), *res->GetContentAsString());
		if (StatusCode == EErrorCode::EmergenceOk) {
			int Nonce = JsonObject.GetObjectField("data")->GetIntegerField("getNonceForChainAddress");
			FString Message = UARTMBuilderLibrary::GenerateARTM(_Message, _ARTMOperations, FuturepassAddress, FString::FromInt(Nonce)).ReplaceCharWithEscapedChar();
			URequestToSign* URequestToSign = URequestToSign::RequestToSign(WorldContextObject, Message);
			UE_LOG(LogEmergenceHttp, Display, TEXT("Sending the following message to Request to Sign: %s"), *Message);
			URequestToSign->OnRequestToSignCompleted.AddDynamic(this, &USendARTMAsync::OnRequestToSignCompleted);
			RequestToSignRequest = URequestToSign->Request;
			URequestToSign->Activate();
			return;
		}
	});
	GetNonceRequest->ProcessRequest();
	
}

void USendARTMAsync::OnRequestToSignCompleted(FString SignedMessage, EErrorCode StatusCode)
{
	UE_LOG(LogTemp, Display, TEXT("OnRequestToSignCompleted: %s"), *SignedMessage);
}
