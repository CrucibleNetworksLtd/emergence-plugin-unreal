// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#include "WebLogin/CustodialWriteTransaction.h"
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

bool UCustodialWriteTransaction::_isServerStarted = false;
FHttpRouteHandle UCustodialWriteTransaction::RouteHandle = nullptr;

UCustodialWriteTransaction* UCustodialWriteTransaction::CustodialWriteTransaction(UObject* WorldContextObject, FString FVCustodialEOA, UEmergenceDeployment* DeployedContract, FString Method)
{
	UCustodialWriteTransaction* BlueprintNode = NewObject<UCustodialWriteTransaction>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	BlueprintNode->FVCustodialEOA = FVCustodialEOA;
	BlueprintNode->DeployedContract = DeployedContract;
	BlueprintNode->Method = Method;
	return BlueprintNode;
}

void UCustodialWriteTransaction::Activate()
{
	if (FVCustodialEOA.IsEmpty() || Method.IsEmpty() || !DeployedContract) {
		UE_LOG(LogTemp, Error, TEXT("Could not do CustodialWriteTransaction, param invalid!"));
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

	if (httpRouter.IsValid() && !UCustodialWriteTransaction::_isServerStarted)
	{

#if(ENGINE_MINOR_VERSION >= 2) && (ENGINE_MAJOR_VERSION >= 5)
		FHttpRequestHandler Handler;
		Handler.BindLambda([this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });
		UCustodialWriteTransaction::RouteHandle = httpRouter->BindRoute(FHttpPath(TEXT("/write-callback")), EHttpServerRequestVerbs::VERB_GET, Handler);

#else
		UCustodialWriteTransaction::RouteHandle = httpRouter->BindRoute(FHttpPath(TEXT("/write-callback")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });
#endif

		httpServerModule.StartAllListeners();

		UCustodialWriteTransaction::_isServerStarted = true;
		UE_LOG(LogTemp, Log, TEXT("Web server started on port = %d"), ServerPort);
	}
	else if (UCustodialWriteTransaction::_isServerStarted) {
		UE_LOG(LogTemp, Log, TEXT("Web already started on port = %d"), ServerPort);
	}
	else
	{
		UCustodialWriteTransaction::_isServerStarted = false;
		UE_LOG(LogTemp, Error, TEXT("Could not start web server on port = %d"), ServerPort);
		return;
	}

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	FString ContentString = "{\"eoa\": \"0xB009d2c5d852FEd6C30511A8F50101957B4F4937\"}";

	UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(
		this,
		&UCustodialWriteTransaction::GetEncodedPayload_HttpRequestComplete,
		"http://localhost:3002/encode-transaction",
		"POST",
		300.0F,
		Headers,
		ContentString);

	/*auto WriteMethodRequest = UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(
		this,
		&UCustodialWriteTransaction::GetEncodedPayload_HttpRequestComplete,
		UHttpHelperLibrary::APIBase + 
		"getEncodedPayloadSC?contractAddress=" + DeployedContract->Address + 
		"&nodeUrl=" + DeployedContract->Blockchain->NodeURL + 
		"&abi=" + DeployedContract->Contract->ABI +
		"&contractAddress=" + DeployedContract->Address +
		"&methodName=" + Method + 
		"&chainId=" + FString::Printf(TEXT("%lld"), DeployedContract->Blockchain->ChainID) + 
		"&value=7500000000000" +
		"&useraddress=" + *FVCustodialEOA,
		"POST",
		300.0F, //give the user lots of time to mess around setting high gas fees
		Headers,
		ContentString.ReplaceCharWithEscapedChar());*/
}

void UCustodialWriteTransaction::BeginDestroy()
{
	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(3000);

	if (httpRouter.IsValid() && UCustodialWriteTransaction::_isServerStarted)
	{
		httpRouter->UnbindRoute(UCustodialWriteTransaction::RouteHandle);
		UCustodialWriteTransaction::_isServerStarted = false;
	}

	UEmergenceAsyncSingleRequestBase::BeginDestroy();
}

void UCustodialWriteTransaction::GetEncodedPayload_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogTemp, Display, TEXT("WriteMethod_HttpRequestComplete"));
	UE_LOG(LogTemp, Display, TEXT("Transaction data: %s"), *HttpResponse->GetContentAsString());

	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject GetEncodedPayloadSCResponse = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString SignerUrl = GetEncodedPayloadSCResponse.GetStringField("fullSignerUrl");
		RawTransactionWithoutSignature = GetEncodedPayloadSCResponse.GetObjectField("rawTransactionWithoutSignature");
		if (!SignerUrl.IsEmpty()) {
			FPlatformProcess::LaunchURL(*SignerUrl, nullptr, nullptr);
		}
		else {
			//@TODO ADD ERROR CODE HERE
		}
	}
	else {
		//@TODO ADD ERROR CODE HERE
	}
	
}

bool UCustodialWriteTransaction::HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
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
		return true;
	}
	FString Signature = JsonParsed->GetObjectField("result")->GetObjectField("data")->GetStringField("signature");
	FString EOA = JsonParsed->GetObjectField("payload")->GetStringField("account");

	TSharedPtr<FJsonObject> RawTransactionObject= MakeShared<FJsonObject>();
	RawTransactionObject->SetObjectField("rawTransactionWithoutSignature", RawTransactionWithoutSignature);
	RawTransactionObject->SetStringField("transactionSignature", Signature);
	RawTransactionObject->SetStringField("fromEoa", EOA);
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RawTransactionObject.ToSharedRef(), Writer);
	UE_LOG(LogTemp, Display, TEXT("RawTransactionObject: %s"), *OutputString);

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(
		this, 
		&UCustodialWriteTransaction::SendTransaction_HttpRequestComplete, 
		"http://localhost:3002/send-transaction", 
		"POST", 
		60.0F, 
		Headers, 
		OutputString);

	return true;
}

void UCustodialWriteTransaction::SendTransaction_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogTemp, Display, TEXT("SendTransaction_HttpRequestComplete"));
	UE_LOG(LogTemp, Display, TEXT("SendTransaction_HttpRequestComplete data: %s"), *HttpResponse->GetContentAsString());
}

TUniquePtr<FHttpServerResponse> UCustodialWriteTransaction::GetHttpPage()
{
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("You may now close this window..."), TEXT("text/html"));
	return response;
}