// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#include "WebLogin/CustodialSignMessage.h"
#include "HttpPath.h"
#include "IHttpRouter.h"
#include "HttpServerHttpVersion.h"
#include "HttpServerModule.h"
#include "HttpServerResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Misc/Base64.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "WalletService/WriteMethod.h"
#include "JwtVerifier.h"
#include "SHA256Hash.h"
#include "Containers/ArrayView.h"
#include <sstream>
#include <iomanip>
#include <string>

bool UCustodialSignMessage::_isServerStarted = false;

UCustodialSignMessage* UCustodialSignMessage::CustodialSignMessage(UObject* WorldContextObject, FString FVCustodialEOA, FString Message)
{
	UCustodialSignMessage* BlueprintNode = NewObject<UCustodialSignMessage>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	BlueprintNode->FVCustodialEOA = FVCustodialEOA;
	BlueprintNode->Message = Message;
	return BlueprintNode;
}

void UCustodialSignMessage::Activate()
{
	if (FVCustodialEOA.IsEmpty() || Message.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("Could not do CustodialSignMessage, param invalid! EOA was \"%s\", message was \"%s\""), *FVCustodialEOA, *Message);
		OnCustodialSignMessageComplete.Broadcast(FString(), EErrorCode::EmergenceClientFailed);
		SetReadyToDestroy();
		return;
	}

	int ServerPort = 3000;

	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(ServerPort);

	if (httpRouter.IsValid() && !UCustodialSignMessage::_isServerStarted)
	{
		httpRouter->BindRoute(FHttpPath(TEXT("/signature-callback")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });

		httpServerModule.StartAllListeners();

		UCustodialSignMessage::_isServerStarted = true;
		UE_LOG(LogTemp, Log, TEXT("Web server started on port = %d"), ServerPort);
	}
	else if (UCustodialSignMessage::_isServerStarted) {
		UE_LOG(LogTemp, Log, TEXT("Web already started on port = %d"), ServerPort);
	}
	else
	{
		UCustodialSignMessage::_isServerStarted = false;
		UE_LOG(LogTemp, Error, TEXT("Could not start web server on port = %d"), ServerPort);
		OnCustodialSignMessageComplete.Broadcast(FString(), EErrorCode::EmergenceClientFailed);
		SetReadyToDestroy();
		return;
	}

	//this segment is to do the same thing as ""0x" + Encoding.UTF8.GetBytes(value).ToHex()" in C#. Make sure if you implement this that it matches that output exactly.
	const char* UTF8Message = TCHAR_TO_UTF8(*Message);
	std::ostringstream oss;
	for (size_t i = 0; i < Message.Len(); ++i) {
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(UTF8Message[i]);
	}
	std::string UTF8MessageHex = "0x" + oss.str();
	
	TSharedPtr<FJsonObject> SignTransactionPayloadJsonObject = MakeShareable(new FJsonObject);
	SignTransactionPayloadJsonObject->SetStringField("account", *FVCustodialEOA);
	SignTransactionPayloadJsonObject->SetStringField("message", UTF8MessageHex.c_str());
	SignTransactionPayloadJsonObject->SetStringField("callbackUrl", "http://localhost:3000/signature-callback");

	TSharedPtr<FJsonObject> EncodedPayloadJsonObject = MakeShareable(new FJsonObject);
	EncodedPayloadJsonObject->SetStringField("id", "client:2"); //must be formatted as `client:${ an identifier number }`
	EncodedPayloadJsonObject->SetStringField("tag", "fv/sign-msg"); //do not change this
	EncodedPayloadJsonObject->SetObjectField("payload", SignTransactionPayloadJsonObject);
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(EncodedPayloadJsonObject.ToSharedRef(), Writer);
	UE_LOG(LogTemp, Display, TEXT("Message to sign is: %s"), *Message);
	UE_LOG(LogTemp, Display, TEXT("GetEncodedPayload OutputString: %s"), *OutputString);
	FString Base64Encode = FBase64::Encode(OutputString);
	UE_LOG(LogTemp, Display, TEXT("GetEncodedPayload Base64Encode: %s"), *Base64Encode);
	FString URL = "https://signer.futureverse.cloud?request=" + Base64Encode;
	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

bool UCustodialSignMessage::HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogTemp, Display, TEXT("HandleSignatureCallback"));
	UHttpHelperLibrary::RequestPrint(Req); //debug logging for the request sent to our local server
	TUniquePtr<FHttpServerResponse> response = GetHttpPage();
	OnComplete(MoveTemp(response));
	FString ResponseBase64 = *Req.QueryParams.Find("response");
	FString ResponseJsonString;
	FBase64::Decode(ResponseBase64, ResponseJsonString);
	UE_LOG(LogTemp, Display, TEXT("HandleSignatureCallback ResponseJsonString: %s"), *ResponseJsonString);
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseJsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		UE_LOG(LogTemp, Error, TEXT("HandleSignatureCallback: Deserialize failed!"));
		OnCustodialSignMessageComplete.Broadcast(FString(), EErrorCode::EmergenceClientJsonParseFailed);
		SetReadyToDestroy();
		return true;
	}
	TSharedPtr<FJsonObject> ResultObject;
	FString Signature = JsonParsed->GetObjectField("result")->GetObjectField("data")->GetStringField("signature");
	OnCustodialSignMessageComplete.Broadcast(Signature, EErrorCode::EmergenceOk);

	SetReadyToDestroy();
	return true;
}

TUniquePtr<FHttpServerResponse> UCustodialSignMessage::GetHttpPage()
{
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("You may now close this window..."), TEXT("text/html"));
	return response;
}