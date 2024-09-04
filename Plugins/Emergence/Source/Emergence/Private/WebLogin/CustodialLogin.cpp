#include "WebLogin/CustodialLogin.h"
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

UCustodialLogin* UCustodialLogin::CustodialLogin(UObject* WorldContextObject)
{
	UCustodialLogin* BlueprintNode = NewObject<UCustodialLogin>();
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

FString UCustodialLogin::CleanupBase64ForWeb(FString Input)
{
	Input.ReplaceCharInline('+', '-');
	Input.ReplaceCharInline('/', '_');
	for (int i = 0; i < Input.Len(); i++) {
		if (Input[i] == '=') {
			Input.RemoveAt(i);
			i--;
		}
	}

	return Input;
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

	if (httpRouter.IsValid()) 
	{
		httpRouter->BindRoute(FHttpPath(TEXT("/callback")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleAuthRequestCallback(Req, OnComplete); });

		httpServerModule.StartAllListeners();
		
		this->_isServerStarted = true;
		UE_LOG(LogTemp, Log, TEXT("Web server started on port = %d"), ServerPort);
	}
	else
	{
		this->_isServerStarted = false;
		UE_LOG(LogTemp, Error, TEXT("Could not start web server on port = %d"), ServerPort);
		OnLoginCompleted.Broadcast(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientFailed);
		return;
	}

	
	
	//create a new state for this interaction
	state = GetSecureRandomBase64();

	//Create a random string to be the "code"
	code = GetSecureRandomBase64();

	//Create a SHA256 of code to be the "code_challange"
	FSHA256Hash SHA256Hash;
	SHA256Hash.FromString(code);

	//convert the output of SHA256Hash.GetHash() from a hex number to a base64 number
	TArray<uint8> UtfChar;
	for (size_t Index_Chars = 0; Index_Chars < SHA256Hash.GetHash().Len(); Index_Chars += 2)
	{
		FString Part = SHA256Hash.GetHash().Mid(Index_Chars, 2);
		char Character = std::stoul(TCHAR_TO_ANSI(*Part), nullptr, 16);
		UtfChar.Add(Character);
	}
	FString CodeChallenge = CleanupBase64ForWeb(FBase64::Encode(UtfChar));

	TArray<TPair<FString, FString>> UrlParams({
		TPair<FString, FString>{"response_type", "code"},
		TPair<FString, FString>{"client_id", clientid},
		TPair<FString, FString>{"redirect_uri", "http%3A%2F%2Flocalhost%3A3000%2Fcallback"},
		TPair<FString, FString>{"scope", "openid"},
		TPair<FString, FString>{"code_challenge", CodeChallenge},
		TPair<FString, FString>{"code_challenge_method", "S256"},
		TPair<FString, FString>{"response_mode", "query"},
		TPair<FString, FString>{"prompt", "login"},
		//TPair<FString, FString>{"prompt", "none"},
		TPair<FString, FString>{"state", FString(state)},
		TPair<FString, FString>{"nonce", GetSecureRandomBase64()},
		TPair<FString, FString>{"login_hint", "social%3Agoogle"},
		});

	//Encode the params in a GET request style
	FString URL = TEXT("https://login.futureverse.cloud/auth?");
	for (int i = 0; i < UrlParams.Num(); i++) {
		URL += UrlParams[i].Key;
		URL += "=";
		URL += UrlParams[i].Value;
		if (i != UrlParams.Num() - 1) {
			URL += "&";
		}
	}

	//Open the auth URL with the params
	FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

bool UCustodialLogin::HandleAuthRequestCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogTemp, Display, TEXT("HandleRequestCallback"));
	TUniquePtr<FHttpServerResponse> response = GetHttpPage();
	RequestPrint(Req);
	OnComplete(MoveTemp(response));


	if (!Req.QueryParams.Contains("code")) {
		UE_LOG(LogTemp, Error, TEXT("HandleRequestCallback: No \"code\""));		
		OnLoginCompleted.Broadcast(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientFailed);
		return true;
	}

	if (*Req.QueryParams.Find("state") != this->state) {
		UE_LOG(LogTemp, Error, TEXT("HandleRequestCallback: \"state\" doesn't match. Returned state was \"%s\", we think it should be \"%s\"."), Req.QueryParams.Find("state"), *this->state);
		OnLoginCompleted.Broadcast(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientFailed);
		return true;
	}

	TArray<TPair<FString, FString>> UrlParams({
		TPair<FString, FString>{"grant_type", "authorization_code"},
		TPair<FString, FString>{"code",* Req.QueryParams.Find("code")},
		TPair<FString, FString>{"redirect_uri", "http%3A%2F%2Flocalhost%3A3000%2Fcallback"},
		TPair<FString, FString>{"client_id", clientid},
		TPair<FString, FString>{"code_verifier", code},
	});

	FString URL = TEXT("https://login.futureverse.cloud/token?");


	//for some reason, the parameters on this request are encoded like a GET url's parameters, but then sent in a POST as part of the content, don't ask me why lol
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
	UE_LOG(LogTemp, Display, TEXT("Sent Params Data: %s"), *Params);
	return true;
}

void UCustodialLogin::GetTokensRequest_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogTemp, Display, TEXT("GetTokensRequest_HttpRequestComplete"));
	UE_LOG(LogTemp, Display, TEXT("Tokens Data: %s"), *HttpResponse->GetContentAsString());
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(*HttpResponse->GetContentAsString());
	if (!FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		UE_LOG(LogTemp, Error, TEXT("GetTokensRequest_HttpRequestComplete: Deserialize failed!"));
		OnLoginCompleted.Broadcast(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientJsonParseFailed);
		return;
	}

	FString IdToken;
	if (!JsonParsed->TryGetStringField("id_token", IdToken)) {
		UE_LOG(LogTemp, Error, TEXT("GetTokensRequest_HttpRequestComplete: Could not get id_token!"));
		UE_LOG(LogTemp, Display, TEXT("code was: %s"), *code);
		OnLoginCompleted.Broadcast(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientJsonParseFailed);
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("id_token: %s"), *IdToken);
	TMap<FString, FString> IdTokenDecoded;
	FJwtVerifierModule JwtVerifier = FJwtVerifierModule::Get();
	IdTokenDecoded = JwtVerifier.GetClaims(IdToken);
	if (IdTokenDecoded.Num() == 0) {
		UE_LOG(LogTemp, Display, TEXT("IdTokenDecoded length was 0"));
		OnLoginCompleted.Broadcast(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientInvalidResponse);
		return;
	}
	OnLoginCompleted.Broadcast(FEmergenceCustodialLoginOutput(IdTokenDecoded), EErrorCode::EmergenceOk);
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

TUniquePtr<FHttpServerResponse> UCustodialLogin::GetHttpPage()
{
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("You may now close this window..."), TEXT("text/html"));
	//FTCHARToUTF8 ConvertToUtf8(TEXT(""));
	//const uint8* ConvertToUtf8Bytes = (reinterpret_cast<const uint8*>(ConvertToUtf8.Get()));
	//response->Body.Append(ConvertToUtf8Bytes, ConvertToUtf8.Length());
	return response;
}

FString UCustodialLogin::GetSecureRandomBase64()
{
	//This should be "cryptographically secure random data" that is base 64 url encoded. I don't know if this is secure enough.
	//@TODO check this is secure enough
	TArray<uint8> Data;
	for (int i = 0; i < 16; i++) { //each these will come out to two characters, so half length
		Data.Add((uint8)FMath::RandHelper(255));
	}
	return CleanupBase64ForWeb(FBase64::Encode(FString::FromHexBlob(Data.GetData(), Data.Num())));
}
