#include "CustodialWriteTransaction.h"
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

UCustodialWriteTransaction* UCustodialWriteTransaction::CustodialWriteTransaction(UObject* WorldContextObject, FString FVCustodialEOA, UEmergenceDeployment* DeployedContract, FString Method)
{
	UCustodialWriteTransaction* BlueprintNode = NewObject<UCustodialWriteTransaction>();
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


	/*
	UEmergenceContract* Contract = NewObject<UEmergenceContract>(UEmergenceContract::StaticClass());
	Contract->ABI = TEXT(R"([{"inputs":[{"internalType":"address","name":"countOf","type":"address"}],"name":"GetCurrentCount","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"IncrementCount","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"","type":"address"}],"name":"currentCount","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"}])");
	UEmergenceChain* Chain = NewObject<UEmergenceChain>(UEmergenceChain::StaticClass());
	Chain->ChainID = 7672;
	Chain->Name = FText::FromString("RootPorcini");
	Chain->NodeURL = "https://porcini.rootnet.app/archive";
	Chain->Symbol = "ROOT";
	UEmergenceDeployment* DeployedContract = NewObject<UEmergenceDeployment>(UEmergenceDeployment::StaticClass());
	DeployedContract->Address = "0x65245508479208091a92d53011c0d24AF28E4163"; //address of the contract we're talking to, not the user address
	DeployedContract->Blockchain = Chain;
	DeployedContract->Contract = Contract;
	*/

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	FString ContentString = "[]";

	auto WriteMethodRequest = UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(
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
		ContentString.ReplaceCharWithEscapedChar());
}

void UCustodialWriteTransaction::GetEncodedPayload_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogTemp, Display, TEXT("WriteMethod_HttpRequestComplete"));
	UE_LOG(LogTemp, Display, TEXT("Transaction data: %s"), *HttpResponse->GetContentAsString());

	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject GetEncodedPayloadSCResponse = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString SerializedUnsignedTransaction = GetEncodedPayloadSCResponse.GetObjectField("message")->GetStringField("serializedUnsignedTransaction");
		TransactionNonce = GetEncodedPayloadSCResponse.GetObjectField("message")->GetStringField("nonce");
		UnsignedTransaction = GetEncodedPayloadSCResponse.GetObjectField("message")->GetStringField("rawUnsignedTransaction");
		TSharedPtr<FJsonObject> SignTransactionPayloadJsonObject = MakeShareable(new FJsonObject);
		SignTransactionPayloadJsonObject->SetStringField("account", *FVCustodialEOA);
		SignTransactionPayloadJsonObject->SetStringField("transaction", *SerializedUnsignedTransaction);
		SignTransactionPayloadJsonObject->SetStringField("callbackUrl", "http://localhost:3000/signature-callback");

		TSharedPtr<FJsonObject> EncodedPayloadJsonObject = MakeShareable(new FJsonObject);
		EncodedPayloadJsonObject->SetStringField("id", "client:2"); //must be formatted as `client:${ an identifier number }`
		EncodedPayloadJsonObject->SetStringField("tag", "fv/sign-tx"); //do not change this
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
	
}

bool UCustodialWriteTransaction::HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
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
		return true;
	}
	FString Signature = JsonParsed->GetObjectField("result")->GetObjectField("data")->GetStringField("signature");
	

	TSharedPtr<FJsonObject> RawTransactionObject;
	TSharedRef<TJsonReader<TCHAR>> RawTransactionJsonReader = TJsonReaderFactory<TCHAR>::Create(UnsignedTransaction);
	if (FJsonSerializer::Deserialize(RawTransactionJsonReader, RawTransactionObject))
	{
		RawTransactionObject->SetStringField("signature", *Signature);
		RawTransactionObject->SetStringField("from", *FVCustodialEOA);
		RawTransactionObject->SetStringField("nonce", *TransactionNonce);

		FString OutputString;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(RawTransactionObject.ToSharedRef(), Writer);
		UE_LOG(LogTemp, Display, TEXT("RawTransactionObject: %s"), *OutputString);
	}
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	Headers.Add(TPair<FString, FString>{"Accept", "application/json"});
	FString Content = "{\"jsonrpc\":\"2.0\",\"method\":\"author_submitExtrinsic\",\"params\":[\"0x5884e6b26c9ab58efd4f380c3d942de02156a375256c89c9da9140bfd1e4898827578ebda4e6b1fb5eae1128aa3949a1062e4c72c5d588cd5d78417f418782821c\"],\"id\":1}";
	UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(this, &UCustodialWriteTransaction::SendTransaction_HttpRequestComplete, "https://porcini.rootnet.app/archive", "POST", 60.0F, Headers, Content);

	return true;
}

void UCustodialWriteTransaction::SendTransaction_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogTemp, Display, TEXT("SendTransaction_HttpRequestComplete"));
	UE_LOG(LogTemp, Display, TEXT("SendTransaction_HttpRequestComplete data: %s"), *HttpResponse->GetContentAsString());
}

void UCustodialWriteTransaction::RequestPrint(const FHttpServerRequest& Req, bool PrintBody)
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

TUniquePtr<FHttpServerResponse> UCustodialWriteTransaction::GetHttpPage()
{
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("You may now close this window..."), TEXT("text/html"));
	//FTCHARToUTF8 ConvertToUtf8(TEXT(""));
	//const uint8* ConvertToUtf8Bytes = (reinterpret_cast<const uint8*>(ConvertToUtf8.Get()));
	//response->Body.Append(ConvertToUtf8Bytes, ConvertToUtf8.Length());
	return response;
}