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
FHttpRouteHandle UCustodialSignMessage::RouteHandle = nullptr;


UCustodialSignMessage* UCustodialSignMessage::CustodialSignMessage(UObject* WorldContextObject, FString FVCustodialEOA, FString Message)
{
	UCustodialSignMessage* BlueprintNode = NewObject<UCustodialSignMessage>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->FVCustodialEOA = FVCustodialEOA;
	BlueprintNode->Message = Message;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UCustodialSignMessage::BeginDestroy()
{
	//if (!this->HasAnyFlags(RF_StrongRefOnFrame)) {
		/*FHttpServerModule& httpServerModule = FHttpServerModule::Get();
		TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(3000);

		if (httpRouter.IsValid() && UCustodialSignMessage::_isServerStarted)
		{
			httpRouter->UnbindRoute(UCustodialSignMessage::RouteHandle);
			UCustodialSignMessage::_isServerStarted = false;
		}
		*/
		UEmergenceAsyncSingleRequestBase::BeginDestroy();
	//}
}

void UCustodialSignMessage::LaunchSignMessageURL()
{
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

	FString URL;
	auto Singleton = UEmergenceSingleton::GetEmergenceManager(this->WorldContextObject);
	check(Singleton);
	EFutureverseEnvironment Env = Singleton->GetFutureverseEnvironment();
	if (Env == EFutureverseEnvironment::Production) {
		URL = TEXT("https://signer.futureverse.app?request=") + Base64Encode;
	}
	else {
		URL = TEXT("https://signer.futureverse.cloud?request=") + Base64Encode;
	}

	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

void UCustodialSignMessage::Activate()
{
	if (FVCustodialEOA.IsEmpty() || Message.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("Could not do CustodialSignMessage, param invalid! EOA was \"%s\", message was \"%s\""), *FVCustodialEOA, *Message);
		OnCustodialSignMessageComplete.Broadcast(FString(), EErrorCode::EmergenceClientFailed);
		SetReadyToDestroy();
		return;
	}

	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(3000);

	if (httpRouter.IsValid() && !UCustodialSignMessage::_isServerStarted)
	{

#if(ENGINE_MINOR_VERSION >= 4) && (ENGINE_MAJOR_VERSION >= 5)
		FHttpRequestHandler Handler;
		Handler.BindLambda([this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });
		UCustodialSignMessage::RouteHandle = httpRouter->BindRoute(FHttpPath(TEXT("/signature-callback")), EHttpServerRequestVerbs::VERB_GET, Handler);

#else
		UCustodialSignMessage::RouteHandle = httpRouter->BindRoute(FHttpPath(TEXT("/signature-callback")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });
#endif

		httpServerModule.StartAllListeners();
		
		FTimerDelegate TimerCallback;
		/*since we seemingly need to wait for the port to open (its async)
		but theres no way to know when thats happened without rewriting the whole
		of the httpserver module, adding a 1 second delay to make fully sure its open.
		1 second should be way more than is nessacery.*/
		TimerCallback.BindLambda([&]
			{
				LaunchSignMessageURL();
			});
		FTimerHandle Handle;
		WorldContextObject->GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, 1.0f, false);

		UCustodialSignMessage::_isServerStarted = true;
		UE_LOG(LogTemp, Log, TEXT("Web server started on port = 3000"));

	}
	else if (UCustodialSignMessage::_isServerStarted) {
		UE_LOG(LogTemp, Log, TEXT("Web already started on port = 3000"));
		LaunchSignMessageURL();
	}
	else
	{
		UCustodialSignMessage::_isServerStarted = false;
		UE_LOG(LogTemp, Error, TEXT("Could not start web server on port = 3000"));
		OnCustodialSignMessageComplete.Broadcast(FString(), EErrorCode::EmergenceClientFailed);
		SetReadyToDestroy();
		return;
	}


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
		return true;
	}
	TSharedPtr<FJsonObject> ResultObject;
	FString Signature = JsonParsed->GetObjectField("result")->GetObjectField("data")->GetStringField("signature");
	OnCustodialSignMessageComplete.Broadcast(Signature, EErrorCode::EmergenceOk);

	return true;
}

TUniquePtr<FHttpServerResponse> UCustodialSignMessage::GetHttpPage()
{
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("You may now close this window..."), TEXT("text/html"));
	return response;
}