#include "CustodialSignMessage.h"
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

UCustodialSignMessage* UCustodialSignMessage::CustodialSignMessage(UObject* WorldContextObject, FString FVCustodialEOA)
{
	UCustodialSignMessage* BlueprintNode = NewObject<UCustodialSignMessage>();
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	BlueprintNode->FVCustodialEOA = FVCustodialEOA;
	return BlueprintNode;
}

void UCustodialSignMessage::Activate()
{
	if (FVCustodialEOA.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("Could not do CustodialSignMessage, param invalid!"));
		return;
	}


	int ServerPort = 3000;
	if (ServerPort <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not start HttpServer, port number must be greater than zero!"));
		return;
	}

	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(ServerPort);

	if (httpRouter.IsValid())
	{
		httpRouter->BindRoute(FHttpPath(TEXT("/signature-callback")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });

		httpServerModule.StartAllListeners();

		this->_isServerStarted = true;
		UE_LOG(LogTemp, Log, TEXT("Web server started on port = %d"), ServerPort);
	}
	else
	{
		this->_isServerStarted = false;
		UE_LOG(LogTemp, Error, TEXT("Could not start web server on port = %d"), ServerPort);
		return;
	}


	FString Message = "Message";
	TSharedPtr<FJsonObject> SignTransactionPayloadJsonObject = MakeShareable(new FJsonObject);
	SignTransactionPayloadJsonObject->SetStringField("account", *FVCustodialEOA);
	SignTransactionPayloadJsonObject->SetStringField("message", *Message);
	SignTransactionPayloadJsonObject->SetStringField("callbackUrl", "http://localhost:3000/signature-callback");

	TSharedPtr<FJsonObject> EncodedPayloadJsonObject = MakeShareable(new FJsonObject);
	EncodedPayloadJsonObject->SetStringField("id", "client:2"); //must be formatted as `client:${ an identifier number }`
	EncodedPayloadJsonObject->SetStringField("tag", "fv/sign-msg"); //do not change this
	EncodedPayloadJsonObject->SetObjectField("payload", SignTransactionPayloadJsonObject);
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(EncodedPayloadJsonObject.ToSharedRef(), Writer);

	UE_LOG(LogTemp, Display, TEXT("GetEncodedPayload OutputString: %s"), *OutputString);
	FString Base64Encode = FBase64::Encode(OutputString);
	UE_LOG(LogTemp, Display, TEXT("GetEncodedPayload Base64Encode: %s"), *Base64Encode);
	FString URL = "https://signer.futureverse.cloud?request=" + Base64Encode;
	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

bool UCustodialSignMessage::HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogTemp, Display, TEXT("HandleSignatureCallback"));
	RequestPrint(Req);
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
		
	}
	return true;
}

void UCustodialSignMessage::RequestPrint(const FHttpServerRequest& Req, bool PrintBody)
{
	FString strRequestType;
	switch (Req.Verb)
	{
	case EHttpServerRequestVerbs::VERB_GET:
		strRequestType = TEXT("GET");
		break;
	case EHttpServerRequestVerbs::VERB_POST:
		strRequestType = TEXT("POST");
		break;
	case EHttpServerRequestVerbs::VERB_PUT:
		strRequestType = TEXT("PUT");
		break;
	default:
		strRequestType = TEXT("Invalid");
	}
	UE_LOG(LogTemp, Log, TEXT("RequestType = '%s'"), *strRequestType);

	HttpVersion::EHttpServerHttpVersion httpVersion{ Req.HttpVersion };
	UE_LOG(LogTemp, Log, TEXT("HttpVersion = '%s'"), *HttpVersion::ToString(httpVersion));

	UE_LOG(LogTemp, Log, TEXT("RelativePath = '%s'"), *Req.RelativePath.GetPath());

	for (const auto& header : Req.Headers)
	{
		FString strHeaderVals;
		for (const auto& val : header.Value)
		{
			strHeaderVals += "'" + val + "' ";
		}
		UE_LOG(LogTemp, Log, TEXT("Header = '%s' : %s"), *header.Key, *strHeaderVals);
	}

	for (const auto& pathParam : Req.PathParams)
	{
		UE_LOG(LogTemp, Log, TEXT("PathParam = '%s' : '%s'"), *pathParam.Key, *pathParam.Value);
	}

	for (const auto& queryParam : Req.QueryParams)
	{
		UE_LOG(LogTemp, Log, TEXT("QueryParam = '%s' : '%s'"), *queryParam.Key, *queryParam.Value);
	}

	// Convert UTF8 to FString
	FUTF8ToTCHAR bodyTCHARData(reinterpret_cast<const ANSICHAR*>(Req.Body.GetData()), Req.Body.Num());
	FString strBodyData{ bodyTCHARData.Length(), bodyTCHARData.Get() };

	UE_LOG(LogTemp, Log, TEXT("Body = '%s'"), *strBodyData);
};

TUniquePtr<FHttpServerResponse> UCustodialSignMessage::GetHttpPage()
{
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("You may now close this window..."), TEXT("text/html"));
	//FTCHARToUTF8 ConvertToUtf8(TEXT(""));
	//const uint8* ConvertToUtf8Bytes = (reinterpret_cast<const uint8*>(ConvertToUtf8.Get()));
	//response->Body.Append(ConvertToUtf8Bytes, ConvertToUtf8.Length());
	return response;
}