#include "CustodialLogin.h"
#include "HttpPath.h"
#include "IHttpRouter.h"
#include "HttpServerHttpVersion.h"
#include "HttpServerModule.h"
#include "HttpServerResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Misc/Base64.h"
#include "Interfaces/IHttpResponse.h"


UCustodialLogin* UCustodialLogin::CustodialLogin(UObject* WorldContextObject)
{
	UCustodialLogin* BlueprintNode = NewObject<UCustodialLogin>();
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UCustodialLogin::GetTokensRequest_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), *HttpResponse->GetContentAsString());
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
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return RequestGET(Req, OnComplete); });
		/*httpRouter->BindRoute(FHttpPath(TEXT("/post")), EHttpServerRequestVerbs::VERB_POST,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return RequestPOST(Req, OnComplete); });
		httpRouter->BindRoute(FHttpPath(TEXT("/put")), EHttpServerRequestVerbs::VERB_PUT,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return RequestPUT(Req, OnComplete); });*/

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
	FString EncodedSig = "8734f600bc77044874826164d38c18af0e12eac3bec037b332bf0029530dbf52";

	TArray<TPair<FString, FString>> UrlParams({ 
		TPair<FString, FString>{"response_type", "code"},
		TPair<FString, FString>{"client_id", clientid},
		TPair<FString, FString>{"redirect_uri", "http%3A%2F%2Flocalhost%3A3000%2Fcallback"},
		TPair<FString, FString>{"scope", "openid"},
		TPair<FString, FString>{"code_challenge", EncodedSig},
		TPair<FString, FString>{"code_challenge_method", "S256"},
		TPair<FString, FString>{"response_mode", "query"},
		TPair<FString, FString>{"prompt", "login"},
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

bool UCustodialLogin::RequestGET(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	RequestPrint(Req);
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HttpServerExample GET"), TEXT("text/html"));
	OnComplete(MoveTemp(response));

	TArray<TPair<FString, FString>> UrlParams({
		TPair<FString, FString>{"grant_type", "authorization_code"},
		TPair<FString, FString>{"code", *Req.QueryParams.Find("code")},
		TPair<FString, FString>{"redirect_uri", "http%3A%2F%2Flocalhost%3A3000%2Fcallback"},
		TPair<FString, FString>{"client_id", clientid},
		TPair<FString, FString>{"code_verifier", code},
	});

	FString URL = TEXT("https://login.futureverse.cloud/token?");
	for (int i = 0; i < UrlParams.Num(); i++) {
		URL += UrlParams[i].Key;
		URL += "=";
		URL += UrlParams[i].Value;
		if (i != UrlParams.Num() - 1) {
			URL += "&";
		}
	}

	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);

	//auto GetTokensRequest = UHttpHelperLibrary::ExecuteHttpRequest<UCustodialLogin>(this, &UCustodialLogin::GetTokensRequest_HttpRequestComplete, URL);

	return true;
}

bool UCustodialLogin::RequestPOST(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	RequestPrint(Req);
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HttpServerExample POST"), TEXT("text/html"));
	OnComplete(MoveTemp(response));
	return true;
}

bool UCustodialLogin::RequestPUT(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	RequestPrint(Req);
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("HttpServerExample PUT"), TEXT("text/html"));
	OnComplete(MoveTemp(response));
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
