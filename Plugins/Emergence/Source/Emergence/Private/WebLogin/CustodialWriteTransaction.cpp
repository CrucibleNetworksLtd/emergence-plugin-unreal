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
TDelegate<void(FString, FString, EErrorCode)> UCustodialWriteTransaction::CallbackComplete;

UCustodialWriteTransaction* UCustodialWriteTransaction::CustodialWriteTransaction(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract, FString Method, FString Value, TArray<FString> Content)
{
	UCustodialWriteTransaction* BlueprintNode = NewObject<UCustodialWriteTransaction>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->DeployedContract = DeployedContract;
	BlueprintNode->Method = Method;
	BlueprintNode->Value = Value;
	BlueprintNode->Content = Content;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UCustodialWriteTransaction::Activate()
{
	if (Method.IsEmpty() || !DeployedContract) {
		UE_LOG(LogEmergence, Error, TEXT("Could not do CustodialWriteTransaction, param invalid!"));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceClientFailed);
		return;
	}


	int ServerPort = 3000;
	if (ServerPort <= 0)
	{
		UE_LOG(LogEmergence, Error, TEXT("Could not start HttpServer, port number must be greater than zero!"));
		return;
	}

	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(ServerPort);

	if (httpRouter.IsValid() && !UCustodialWriteTransaction::_isServerStarted)
	{

#if(ENGINE_MINOR_VERSION >= 4) && (ENGINE_MAJOR_VERSION >= 5)
		FHttpRequestHandler Handler;
		Handler.BindLambda([this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });
		httpRouter->BindRoute(FHttpPath(TEXT("/write-callback")), EHttpServerRequestVerbs::VERB_GET, Handler);

#else
		httpRouter->BindRoute(FHttpPath(TEXT("/write-callback")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });
#endif

		httpServerModule.StartAllListeners();

		UCustodialWriteTransaction::_isServerStarted = true;
		UE_LOG(LogEmergence, Log, TEXT("Web server started on port = %d"), ServerPort);

		FTimerDelegate TimerCallback;
		/*since we seemingly need to wait for the port to open (its async) 
		but theres no way to know when thats happened without rewriting the whole
		of the httpserver module, adding a 1 second delay to make fully sure its open. 
		1 second should be way more than is nessacery.*/
		TimerCallback.BindLambda([&]
		{
			GetEncodedData();
		});
		FTimerHandle Handle;
		WorldContextObject->GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, 1.0f, false);
	}
	else if (UCustodialWriteTransaction::_isServerStarted) {
		UE_LOG(LogEmergence, Log, TEXT("Web already started on port = %d"), ServerPort);
		GetEncodedData();
	}
	else
	{
		UCustodialWriteTransaction::_isServerStarted = false;
		UE_LOG(LogEmergence, Error, TEXT("Could not start web server on port = %d"), ServerPort);
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		return;
	}
}

void UCustodialWriteTransaction::GetEncodedData()
{
	TSharedPtr<FJsonObject> JsonToSend = MakeShared<FJsonObject>();
	JsonToSend->SetStringField("ABI", DeployedContract->Contract->ABI);
	JsonToSend->SetStringField("ContractAddress", DeployedContract->Address);
	JsonToSend->SetStringField("MethodName", Method);
	TArray<TSharedPtr<FJsonValue>> CallInputsJsonArray;
	for (FString Param : Content) {
		CallInputsJsonArray.Add(MakeShared<FJsonValueString>(Param));
	}
	JsonToSend->SetArrayField("CallInputs", CallInputsJsonArray);
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonToSend.ToSharedRef(), Writer);
	UE_LOG(LogEmergence, Display, TEXT("GetEncodedData()\n%s"), *OutputString);

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(
		this,
		&UCustodialWriteTransaction::GetEncodedData_HttpRequestComplete,
		UHttpHelperLibrary::APIBase + "getEncodedData",
		"POST",
		60.0F,
		Headers,
		OutputString);
}

void UCustodialWriteTransaction::GetEncodedData_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogEmergence, Display, TEXT("GetEncodedData_HttpRequestComplete:\n%s"), *HttpResponse->GetContentAsString());

	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject GetEncodedDataJson = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString Data = GetEncodedDataJson.GetObjectField("message")->GetStringField("Data");
		FString ChainID = FString::FromInt(DeployedContract->Blockchain->ChainID);
		FString ContractAddress = DeployedContract->Address;
		auto Singleton = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
		if (Singleton) {
			EncodeTransaction(Singleton->GetCachedAddress(true), ChainID, ContractAddress, Value, Data, DeployedContract->Blockchain->NodeURL);
		}
	}
	else {
		UE_LOG(LogEmergence, Error, TEXT("GetEncodedData_HttpRequestComplete: Failed!"));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), StatusCode);
		return;
	}
}

void UCustodialWriteTransaction::EncodeTransaction(FString Eoa, FString ChainId, FString ToAddress, FString InputValue, FString Data, FString InputRpcUrl)
{
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	if (Eoa.IsEmpty() ||
		ChainId.IsEmpty() ||
		ToAddress.IsEmpty() ||
		InputValue.IsEmpty() ||
		Data.IsEmpty() ||
		InputRpcUrl.IsEmpty()) {
		UE_LOG(LogEmergence, Error, TEXT("One of the required inputs to EncodeTransaction() was empty."));
		UE_LOG(LogEmergence, Error, TEXT("Eoa = %s"), *Eoa);
		UE_LOG(LogEmergence, Error, TEXT("ChainId = %s"), *ChainId);
		UE_LOG(LogEmergence, Error, TEXT("ToAddress = %s"), *ToAddress);
		UE_LOG(LogEmergence, Error, TEXT("InputValue = %s"), *InputValue);
		UE_LOG(LogEmergence, Error, TEXT("Data = %s"), *Data);
		UE_LOG(LogEmergence, Error, TEXT("InputRpcUrl = %s"), *InputRpcUrl);
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		return;
	}

	TSharedPtr<FJsonObject> JsonInputs = MakeShared<FJsonObject>();
	JsonInputs->SetStringField("eoa", Eoa);
	JsonInputs->SetStringField("chainId", ChainId);
	JsonInputs->SetStringField("toAddress", ToAddress);
	JsonInputs->SetStringField("value", InputValue);
	JsonInputs->SetStringField("data", Data);
	JsonInputs->SetStringField("rpcUrl", InputRpcUrl);
	UCustodialWriteTransaction::RpcUrl = InputRpcUrl;
	FString JsonInputsString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonInputsString);
	FJsonSerializer::Serialize(JsonInputs.ToSharedRef(), Writer);
	UE_LOG(LogEmergence, Display, TEXT("JsonInputsString: %s"), *JsonInputsString);
	UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(
		this,
		&UCustodialWriteTransaction::GetEncodedPayload_HttpRequestComplete,
		"https://fvhelperservice.openmeta.xyz/encode-transaction",
		"POST",
		300.0F,
		Headers,
		JsonInputsString);
}

void UCustodialWriteTransaction::TransactionEnded()
{
	SetReadyToDestroy();
}

void UCustodialWriteTransaction::GetEncodedPayload_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogEmergence, Display, TEXT("GetEncodedPayload_HttpRequestComplete"));
	UE_LOG(LogEmergence, Display, TEXT("Transaction data: %s"), *HttpResponse->GetContentAsString());

	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject GetEncodedPayloadResponse = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString SignerUrl = GetEncodedPayloadResponse.GetStringField("fullSignerUrl");
		RawTransactionWithoutSignature = *GetEncodedPayloadResponse.GetObjectField("rawTransactionWithoutSignature").Get();
		if (!SignerUrl.IsEmpty()) {
			CallbackComplete.BindLambda([&](FString Signature, FString EOA, EErrorCode ErrorCode) { //this is the callback triggered by the handler of the below launchURL
				if(ErrorCode == EErrorCode::EmergenceOk){
					SendTranscation(Signature, EOA);
				}
				else {
					UE_LOG(LogEmergence, Warning, TEXT("GetEncodedPayload_HttpRequestComplete: failed."));
					OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
				}
			});
			FString Error;
			FPlatformProcess::LaunchURL(*SignerUrl, nullptr, &Error);
			if (!Error.IsEmpty()) {
				UE_LOG(LogEmergence, Display, TEXT("LaunchURL: failed, %s"), *Error);
				OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
				return;
			}
		}
		else {
			UE_LOG(LogEmergence, Display, TEXT("GetEncodedPayload_HttpRequestComplete: failed, signer URL empty."));
			OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
			return;
		}
	}
	else {
		UE_LOG(LogEmergence, Display, TEXT("GetEncodedPayload_HttpRequestComplete: failed"));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), StatusCode);
		return;
	}
	
}

bool UCustodialWriteTransaction::HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogEmergence, Display, TEXT("HandleSignatureCallback"));
	UHttpHelperLibrary::RequestPrint(Req); //debug logging for the request sent to our local server
	TUniquePtr<FHttpServerResponse> response = GetHttpPage();
	OnComplete(MoveTemp(response));

	FString ResponseBase64 = *Req.QueryParams.Find("response");
	FString ResponseJsonString;
	FBase64::Decode(ResponseBase64, ResponseJsonString);
	UE_LOG(LogEmergence, Display, TEXT("HandleSignatureCallback ResponseJsonString: %s"), *ResponseJsonString);
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseJsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		UE_LOG(LogEmergence, Error, TEXT("HandleSignatureCallback: Deserialize failed!"));
		CallbackComplete.Execute(FString(), FString(), EErrorCode::EmergenceClientJsonParseFailed);
		return true;
	}

	if (JsonParsed->GetObjectField("result")->GetStringField("status") != "error") { //if this wasn't an error
		FString Signature = JsonParsed->GetObjectField("result")->GetObjectField("data")->GetStringField("signature");
		FString EOA = JsonParsed->GetObjectField("payload")->GetStringField("account");

		UE_LOG(LogTemp, Display, TEXT("HandleSignatureCallback ResponseJsonString: OnCustodialSignMessageComplete"));
		CallbackComplete.Execute(Signature, EOA, EErrorCode::EmergenceOk);
	}
	else { //if this was an error
		FString ErrorString = JsonParsed->GetObjectField("result")->GetObjectField("data")->GetStringField("error");
		UE_LOG(LogTemp, Display, TEXT("HandleSignatureCallback error: %s"), *ErrorString);
		if (ErrorString == "USER_REJECTED") {
			CallbackComplete.Execute(FString(), FString(), EErrorCode::EmergenceClientUserRejected);
		}
		else {
			CallbackComplete.Execute(FString(), FString(), EErrorCode::ServerError);
		}
	}
	return true;
}

void UCustodialWriteTransaction::SendTranscation(FString Signature, FString EOA)
{
	if (RawTransactionWithoutSignature.Values.Num() == 0) {
		UE_LOG(LogEmergence, Error, TEXT("HandleSignatureCallback: RawTransactionWithoutSignature was null."));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		return;
	}

	if (Signature.IsEmpty() || EOA.IsEmpty() || RpcUrl.IsEmpty()) {
		UE_LOG(LogEmergence, Error, TEXT("HandleSignatureCallback: one of Signature, EOA or RpcUrl was empty."));
		UE_LOG(LogEmergence, Error, TEXT("Signature = %s"), *Signature);
		UE_LOG(LogEmergence, Error, TEXT("EOA = %s"), *EOA);
		UE_LOG(LogEmergence, Error, TEXT("RpcUrl = %s"), *RpcUrl);
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		return;
	}

	TSharedPtr<FJsonObject> RawTransactionObject = MakeShared<FJsonObject>();
	RawTransactionObject->SetObjectField("rawTransactionWithoutSignature", MakeShared<FJsonObject>(RawTransactionWithoutSignature));
	RawTransactionObject->SetStringField("transactionSignature", Signature);
	RawTransactionObject->SetStringField("fromEoa", EOA);
	RawTransactionObject->SetStringField("rpcUrl", RpcUrl);
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RawTransactionObject.ToSharedRef(), Writer);
	UE_LOG(LogEmergence, Display, TEXT("RawTransactionObject: %s"), *OutputString);

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(
		this,
		&UCustodialWriteTransaction::SendTransaction_HttpRequestComplete,
		"https://fvhelperservice.openmeta.xyz/send-transaction",
		"POST",
		300.0F, //Give the server a bit longer to send it, as we're waiting for the blockchain here
		Headers,
		OutputString);
}

void UCustodialWriteTransaction::SendTransaction_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogEmergence, Display, TEXT("SendTransaction_HttpRequestComplete"));
	UE_LOG(LogEmergence, Display, TEXT("SendTransaction_HttpRequestComplete data: %s"), *HttpResponse->GetContentAsString());
	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject GetEncodedPayloadResponse = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString Hash = GetEncodedPayloadResponse.GetStringField("hash");
		UE_LOG(LogEmergence, Display, TEXT("SendTransaction_HttpRequestComplete hash: %s"), *Hash);
		OnCustodialWriteTransactionCompleted.Broadcast(Hash, EErrorCode::EmergenceOk);
		TransactionEnded();
	}
	else {
		UE_LOG(LogEmergence, Error, TEXT("SendTransaction_HttpRequestComplete: failed!"));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), StatusCode);
		return;
	}
}

TUniquePtr<FHttpServerResponse> UCustodialWriteTransaction::GetHttpPage()
{
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("You may now close this window..."), TEXT("text/html"));
	return response;
}