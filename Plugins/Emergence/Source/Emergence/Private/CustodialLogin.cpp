#include "CustodialLogin.h"
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


UCustodialLogin* UCustodialLogin::CustodialLogin(UObject* WorldContextObject)
{
	UCustodialLogin* BlueprintNode = NewObject<UCustodialLogin>();
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UCustodialLogin::Activate()
{
	int ServerPort = 3000;

	if (ServerPort <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not start HttpServer, port number must be greater than zero!"));
		return;
	}

	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(ServerPort);

	// If port already binded by another process, then this check must be failed
	// !!! BUT !!!
	// this check always true
	// I don't no why...
	if (httpRouter.IsValid())
	{
		// You can bind as many routes as you need
		httpRouter->BindRoute(FHttpPath(TEXT("/callback")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleRequestCallback(Req, OnComplete); });

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

	/*FSHA256Signature Sig;
	if (!FPlatformMisc::GetSHA256Signature(*code, code.Len(), Sig)) {
		UE_LOG(LogTemp, Error, TEXT("No GetSHA256Signature implementation"), ServerPort);
		return;
	};

	FString EncodedSig = FBase64::Encode(TArray<uint8>(Sig.Signature, 32));*/
	FString EncodedSig = "8t7T5W9J6npzIhQ4IatD5Kg0Tf10wukKAbIPPolsscI";

	TArray<TPair<FString, FString>> UrlParams({
		TPair<FString, FString>{"response_type", "code"},
		TPair<FString, FString>{"client_id", clientid},
		TPair<FString, FString>{"redirect_uri", "http%3A%2F%2Flocalhost%3A3000%2Fcallback"},
		TPair<FString, FString>{"scope", "openid"},
		TPair<FString, FString>{"code_challenge", EncodedSig},
		TPair<FString, FString>{"code_challenge_method", "S256"},
		TPair<FString, FString>{"response_mode", "query"},
		TPair<FString, FString>{"prompt", "login"},
		//TPair<FString, FString>{"prompt", "none"},
		TPair<FString, FString>{"state", state},
		TPair<FString, FString>{"nonce", "WuMLYhr4RUqVcL05"},
		TPair<FString, FString>{"login_hint", "social%3Agoogle"},
		});

	FString URL = TEXT("https://login.futureverse.cloud/auth?");
	for (int i = 0; i < UrlParams.Num(); i++) {
		URL += UrlParams[i].Key;
		URL += "=";
		URL += UrlParams[i].Value;
		if (i != UrlParams.Num() - 1) {
			URL += "&";
		}
	}

	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

bool UCustodialLogin::HandleRequestCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogTemp, Display, TEXT("HandleRequestCallback"));
	RequestPrint(Req);
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HandleRequestCallback GET"), TEXT("text/html"));
	OnComplete(MoveTemp(response));


	if (!Req.QueryParams.Contains("code")) {
		UE_LOG(LogTemp, Error, TEXT("HandleRequestCallback: No \"code\""));
		return true;
	}

	if (*Req.QueryParams.Find("state") != state) {
		UE_LOG(LogTemp, Error, TEXT("HandleRequestCallback: No \"state\""));
		return true;
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetStringField("grant_type", "authorization_code");
	JsonObject->SetStringField("code", *Req.QueryParams.Find("code"));
	JsonObject->SetStringField("redirect_uri", "http%3A%2F%2Flocalhost%3A3000%2Fcallback");
	JsonObject->SetStringField("client_id", clientid);
	JsonObject->SetStringField("code_verifier", code);

	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TArray<TPair<FString, FString>> UrlParams({
		TPair<FString, FString>{"grant_type", "authorization_code"},
		TPair<FString, FString>{"code",* Req.QueryParams.Find("code")},
		TPair<FString, FString>{"redirect_uri", "http%3A%2F%2Flocalhost%3A3000%2Fcallback"},
		TPair<FString, FString>{"client_id", clientid},
		TPair<FString, FString>{"code_verifier", code},
		});

	FString URL = TEXT("https://login.futureverse.cloud/token?");

	FString Params;
	for (int i = 0; i < UrlParams.Num(); i++) {
		Params += UrlParams[i].Key;
		Params += "=";
		Params += UrlParams[i].Value;
		if (i != UrlParams.Num() - 1) {
			Params += "&";
		}
	}

	TArray<TPair<FString, FString>> Headers({
		TPair<FString, FString>{"Content-Type", "application/x-www-form-urlencoded"}
		});


	UHttpHelperLibrary::ExecuteHttpRequest<UCustodialLogin>(this, &UCustodialLogin::GetTokensRequest_HttpRequestComplete, URL, TEXT("POST"), 60.0F, Headers, Params);
	return true;
}

void UCustodialLogin::GetTokensRequest_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogTemp, Display, TEXT("GetTokensRequest_HttpRequestComplete"));
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(*HttpResponse->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		FString IdToken, IdTokenDecoded;
		if (JsonParsed->TryGetStringField("id_token", IdToken)) {
			UE_LOG(LogTemp, Display, TEXT("id_token: %s"), *IdToken);
			FBase64::Decode(IdToken, IdTokenDecoded);
			UE_LOG(LogTemp, Display, TEXT("id_token decoded: %s"), *IdTokenDecoded);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Tokens Data: %s"), *HttpResponse->GetContentAsString());

	UEmergenceContract* Contract = NewObject<UEmergenceContract>(UEmergenceContract::StaticClass());
	Contract->ABI = TEXT(R"([{"inputs":[{"internalType":"address","name":"countOf","type":"address"}],"name":"GetCurrentCount","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"IncrementCount","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"","type":"address"}],"name":"currentCount","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"}])");
	UEmergenceChain* Chain = NewObject<UEmergenceChain>(UEmergenceChain::StaticClass());
	Chain->ChainID = 7672;
	Chain->Name = FText::FromString("RootPorcini");
	Chain->NodeURL = "https://porcini.rootnet.app/archive";
	Chain->Symbol = "ROOT";
	UEmergenceDeployment* DeployedContract = NewObject<UEmergenceDeployment>(UEmergenceDeployment::StaticClass());
	DeployedContract->Address = "0x65245508479208091a92d53011c0d24AF28E4163";
	DeployedContract->Blockchain = Chain;
	DeployedContract->Contract = Contract;

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	FString ContentString = "[]";

	auto WriteMethodRequest = UHttpHelperLibrary::ExecuteHttpRequest<UCustodialLogin>(
		this,
		&UCustodialLogin::WriteMethod_HttpRequestComplete,
		"http://localhost:5173/api/getEncodedPayloadSC?contractAddress=" + 
		DeployedContract->Address + "&nodeUrl=" + 
		DeployedContract->Blockchain->NodeURL + 
		"&abi=" + Contract->ABI +
		"&contractAddress=" + DeployedContract->Address +
		"&methodName=IncrementCount" + 
		"&chainId=" + FString::Printf(TEXT("%lld"), DeployedContract->Blockchain->ChainID) + 
		"&value=7500000000000" +
		"&nodeurl=https%3A%2F%2Fporcini.rootnet.app%2Farchive" +
		"&useraddress=0x238678df4F2CeeCC8b09C2b49eb94458682e6A4C",
		"POST",
		300.0F, //give the user lots of time to mess around setting high gas fees
		Headers,
		ContentString.ReplaceCharWithEscapedChar());
}

void UCustodialLogin::WriteMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogTemp, Display, TEXT("WriteMethod_HttpRequestComplete"));
	UE_LOG(LogTemp, Display, TEXT("Transaction data: %s"), *HttpResponse->GetContentAsString());
	
	TSharedPtr<FJsonObject> SignTransactionPayloadJsonObject = MakeShareable(new FJsonObject);
	SignTransactionPayloadJsonObject->SetStringField("account", "0x238678df4F2CeeCC8b09C2b49eb94458682e6A4C"); //@TODO change
	SignTransactionPayloadJsonObject->SetStringField("transaction", *HttpResponse->GetContentAsString());
	SignTransactionPayloadJsonObject->SetStringField("callbackUrl", "http://localhost:3000/signature-callback");

	TSharedPtr<FJsonObject> EncodedPayloadJsonObject = MakeShareable(new FJsonObject);
	EncodedPayloadJsonObject->SetStringField("id", "client:2"); //must be formatted as `client:${ an identifier number }`
	EncodedPayloadJsonObject->SetStringField("tag", "fv/sign-tx"); //do not change this
	EncodedPayloadJsonObject->SetObjectField("payload", SignTransactionPayloadJsonObject);
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(EncodedPayloadJsonObject.ToSharedRef(), Writer);

	UE_LOG(LogTemp, Display, TEXT("%s"), *OutputString);
	FString Base64Encode = FBase64::Encode(OutputString);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Base64Encode);
	FString URL = "https://signer.futureverse.cloud?request=" + Base64Encode;
	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

	
}

bool UCustodialLogin::HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogTemp, Display, TEXT("HandleSignatureCallback"));
	RequestPrint(Req);
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HandleSignatureCallback GET"), TEXT("text/html"));
	OnComplete(MoveTemp(response));
	FString ResponseBase64 = *Req.QueryParams.Find("response");
	FString ResponseJsonString;
	FBase64::Decode(ResponseBase64, ResponseJsonString);
	UE_LOG(LogTemp, Display, TEXT("%s"), *ResponseJsonString);
	return true;
}

void UCustodialLogin::RequestPrint(const FHttpServerRequest& Req, bool PrintBody)
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
