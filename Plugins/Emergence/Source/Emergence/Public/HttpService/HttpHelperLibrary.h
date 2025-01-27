// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Misc/EngineVersion.h"
#include "Interfaces/IPluginManager.h"
#include "EmergenceSingleton.h"
#include "PlatformHttp.h"
#include "Kismet/GameplayStatics.h"
#include "EmergenceEVMServerSubsystem.h"
#include "Misc/EngineVersionComparison.h"
#include "HttpServerModule.h"
#include "Types/EmergenceErrorCode.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UHttpHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	inline static FString APIBase = "https://evm7.openmeta.xyz/api/";

	inline static FString EmergenceVersionNumberCache;

	inline static FString GetEmergenceVersionNumber() {
		if (UHttpHelperLibrary::EmergenceVersionNumberCache.IsEmpty()) { //if the cache is empty
			IPluginManager& PluginManager = IPluginManager::Get();
			TArray<TSharedRef<IPlugin>> Plugins = PluginManager.GetDiscoveredPlugins();
			for (const TSharedRef<IPlugin>& Plugin : Plugins) {
				if (Plugin->GetName() == "Emergence") {
					const FPluginDescriptor& Descriptor = Plugin->GetDescriptor();
					UHttpHelperLibrary::EmergenceVersionNumberCache = Descriptor.VersionName;
					return UHttpHelperLibrary::EmergenceVersionNumberCache;
				}
			}
			return "error";
		}
		else { //if we have it already
			return UHttpHelperLibrary::EmergenceVersionNumberCache;
		}
	}

	inline static FString GetFVEnvironment() {
#if UE_BUILD_SHIPPING
		FString Environment = "Production"; //Shipping defaults to production
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseShippingEnvironment"), Environment, GGameIni);
#else
		FString Environment = "Production"; //Everything else defaults to Production as well
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseDevelopmentEnvironment"), Environment, GGameIni);
#endif
		return Environment;
	}

	//Our service for helping sending custodial transactions
	inline static FString GetFVHelperServiceURL() {
		return "https://fvhelperservice.openmeta.xyz/";
	}

	inline static FString GetInventoryServiceAPIURL() {
		return "https://" + GetInventoryServiceHostURL() + "/InventoryService/";
	}

	inline static FString GetAvatarServiceAPIURL() {		
		return "https://" + GetAvatarServiceHostURL() + "/AvatarSystem/";
	}

	//The URL to send the user to to create a futurepass depending on environment
	UFUNCTION(BlueprintPure, Category="Emergence Internal|UI")
	static FString GetFutureverseCreateFuturepassURL() {

		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			//Production Env URL
			return "https://futurepass.futureverse.app/";
		}

		//Staging Env URL
		return "https://identity-dashboard.futureverse.cloud/";
	}

	//the graphQL of the futureverse asset registery depending on environment
	inline static FString GetFutureverseAssetRegistryAPIURL() {

		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			//Production Env URL
			return "https://ar-api.futureverse.app/graphql";
		}

		if (Environment == "Development") {
			//Development Env URL
			return "https://ar-api.futureverse.dev/graphql";
		}

		//Staging Env URL
		return "https://ar-api.futureverse.cloud/graphql";
	}

	//the chain id to use with futurepasses depending on environment
	inline static FString GetFutureverseFuturepassChainId() {

		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			//Production Env URL
			return "1";
		}

		//Staging & Dev Env URL
		return "11155111";
	}

	//the location of the futurepass API  depending on environment
	inline static FString GetFutureverseFuturepassAPIURL() {

		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			//Production Env URL
			return "https://account-indexer.pass.online/api/v1";
		}

		//Staging and Dev same Env URL
		return "https://account-indexer.passonline.dev/api/v1";
	}

	//the futureverse signer url depending on environment
	inline static FString GetFutureverseSignerURL() {
		
		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			return TEXT("https://signer.pass.online");
		}
		else {
			return TEXT("https://signer.passonline.cloud");
		}
	}

	//the futureverse auth url depending on environment
	inline static FString GetFutureverseAuthURL() {

		FString Environment = GetFVEnvironment();

		if (Environment == "Production") {
			return TEXT("https://login.pass.online");
		}
		else {
			return TEXT("https://login.passonline.cloud");
		}
	}

	//our inventory service depending on environment
	inline static FString GetInventoryServiceHostURL() {

#if UE_BUILD_SHIPPING
		FString DevelopmentEnvironmentString = "Production";
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("DevelopmentEnvironment"), DevelopmentEnvironmentString, GGameIni);
#else
		FString DevelopmentEnvironmentString = "Staging";
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("ShippingEnvironment"), DevelopmentEnvironmentString, GGameIni);
#endif

		bool IsDevelopmentMode = false;
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("EnableDevelopmentEnvironment"), IsDevelopmentMode, GGameIni);

		if (IsDevelopmentMode) {
			return "dysaw5zhak.us-east-1.awsapprunner.com"; //@hack, this is the staging URL because currently there is no dev url. This needs updating
		}

		if (DevelopmentEnvironmentString == "Production") {
			return "7vz9y7rdpy.us-east-1.awsapprunner.com";
		}

		return "dysaw5zhak.us-east-1.awsapprunner.com";
	}

	//our avatar service depending on environment
	inline static FString GetAvatarServiceHostURL() {

#if UE_BUILD_SHIPPING
		FString DevelopmentEnvironmentString = "Production";
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("DevelopmentEnvironment"), DevelopmentEnvironmentString, GGameIni);
#else
		FString DevelopmentEnvironmentString = "Staging";
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("ShippingEnvironment"), DevelopmentEnvironmentString, GGameIni);
#endif

		bool IsDevelopmentMode = false;
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("EnableDevelopmentEnvironment"), IsDevelopmentMode, GGameIni);

		if (IsDevelopmentMode) {
			return "dysaw5zhak.us-east-1.awsapprunner.com"; //@hack, this is the staging URL because currently there is no dev url. This needs updating
		}

		if (DevelopmentEnvironmentString == "Production") {
//			return "7vz9y7rdpy.us-east-1.awsapprunner.com";
			return "dysaw5zhak.us-east-1.awsapprunner.com"; //@hack, only staging is used right now
		}

		return "dysaw5zhak.us-east-1.awsapprunner.com";
	}

	inline static FString InternalIPFSURLToHTTP(FString IPFSURL) {

		if (IPFSURL.Contains(TEXT("ipfs://")) || IPFSURL.Contains(TEXT("IPFS://")) || IPFSURL.Contains(TEXT("https://cloudflare-ipfs.com/ipfs/"))) {
			UE_LOG(LogEmergenceHttp, Display, TEXT("Found %s URL, replacing with public node..."), IPFSURL.Contains(TEXT("https://cloudflare-ipfs.com/ipfs/")) ? TEXT("Cloudflare-IPFS") : TEXT("IPFS"));

			FString IPFSNode = TEXT("http://ipfs.openmeta.xyz/ipfs/");
			FString CustomIPFSNode = "";
			if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("IPFSNode"), CustomIPFSNode, GGameIni))
			{
				if (CustomIPFSNode != "") {
					UE_LOG(LogEmergenceHttp, Display, TEXT("Found custom IPFS node in game config, replacing with \"%s\""), *CustomIPFSNode);
					IPFSNode = CustomIPFSNode;
				}
			}
			FString NewURL = IPFSURL.Replace(TEXT("ipfs://"), *IPFSNode)
				.Replace(TEXT("IPFS://"), *IPFSNode)
				.Replace(TEXT("https://cloudflare-ipfs.com/ipfs/"), *IPFSNode);
			UE_LOG(LogEmergenceHttp, Display, TEXT("New URL is \"%s\""), *NewURL);
			return NewURL;
		}
		else {
			return IPFSURL;
		}
	}

	/**
	 * Takes an IPFS URL and changes it to be a IPFS gateway link (IPFS via HTTP). If its already HTTP, no conversion happens.
	 * @param IPFSURL The URL to try to convert. If its already HTTP, no conversion happens.
	 */
	UFUNCTION(BlueprintPure, Category = "Emergence|Helpers")
	static FString IPFSURLToHTTP(FString IPFSURL) {
		return UHttpHelperLibrary::InternalIPFSURLToHTTP(IPFSURL);
	}

	static void RequestPrint(const FHttpServerRequest& Req, bool PrintBody = true)
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
		UE_LOG(LogEmergenceHttp, Log, TEXT("RequestType = '%s'"), *strRequestType);

		HttpVersion::EHttpServerHttpVersion httpVersion{ Req.HttpVersion };
		UE_LOG(LogEmergenceHttp, Log, TEXT("HttpVersion = '%s'"), *HttpVersion::ToString(httpVersion));

		UE_LOG(LogEmergenceHttp, Log, TEXT("RelativePath = '%s'"), *Req.RelativePath.GetPath());

		for (const auto& header : Req.Headers)
		{
			FString strHeaderVals;
			for (const auto& val : header.Value)
			{
				strHeaderVals += "'" + val + "' ";
			}
			UE_LOG(LogEmergenceHttp, Log, TEXT("Header = '%s' : %s"), *header.Key, *strHeaderVals);
		}

		for (const auto& pathParam : Req.PathParams)
		{
			UE_LOG(LogEmergenceHttp, Log, TEXT("PathParam = '%s' : '%s'"), *pathParam.Key, *pathParam.Value);
		}

		for (const auto& queryParam : Req.QueryParams)
		{
			UE_LOG(LogEmergenceHttp, Log, TEXT("QueryParam = '%s' : '%s'"), *queryParam.Key, *queryParam.Value);
		}

		// Convert UTF8 to FString
		FUTF8ToTCHAR bodyTCHARData(reinterpret_cast<const ANSICHAR*>(Req.Body.GetData()), Req.Body.Num());
		FString strBodyData{ bodyTCHARData.Length(), bodyTCHARData.Get() };

		UE_LOG(LogEmergenceHttp, Log, TEXT("Body = '%s'"), *strBodyData);
	};

	template<typename T>
	inline static TSharedRef<IHttpRequest, ESPMode::ThreadSafe> ExecuteHttpRequest(T* FunctionBindObject, void(T::* FunctionBindFunction)(FHttpRequestPtr, FHttpResponsePtr, bool), const FString& URL, const FString& Verb = TEXT("GET"), const float& Timeout = 60.0F, const TArray<TPair<FString, FString>>& Headers = TArray<TPair<FString, FString>>(), const FString& Content = FString(), const bool ProcessRequestInstantly = true)
	{
		static_assert(std::is_base_of<UObject, T>::value, "T not derived from UObject");

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

		if (GEngine && static_cast<UObject*>(FunctionBindObject)) {
			UObject* WorldContextObject = static_cast<UObject*>(FunctionBindObject);
			if (WorldContextObject) {
				UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
				if (
					World && //get the world
					World->GetGameInstance()) { //if we actually got a world, get the game instance
					UEmergenceEVMServerSubsystem* EmergenceSubsystem = World->GetGameInstance()->GetSubsystem<UEmergenceEVMServerSubsystem>();
					if (EmergenceSubsystem) {
						EmergenceSubsystem->ActiveRequests.Add(HttpRequest);
					}
				};
			}
		}

		if (FunctionBindFunction && FunctionBindObject) {
			HttpRequest->OnProcessRequestComplete().BindUObject(FunctionBindObject, FunctionBindFunction);
		}

		if (URL.IsEmpty()) {
			UE_LOG(LogEmergenceHttp, Warning, TEXT("Tried to ExecuteHttpRequest but URL was empty"));
			return HttpRequest;
		}

		FString FinalURL;

		//switch IPFS to our public node...
		FinalURL = UHttpHelperLibrary::InternalIPFSURLToHTTP(URL);
		HttpRequest->SetURL(FinalURL);
		HttpRequest->SetVerb(Verb);
		HttpRequest->SetTimeout(Timeout);
#if UE_VERSION_NEWER_THAN(5, 4, 0)
		HttpRequest->SetActivityTimeout(Timeout);
#endif
		//Handle headers and logging of the headers
		FString HeaderLogText;
		if (Headers.Num() > 0) {
			HeaderLogText = "\nAdditional headers:\n";
			for (int i = 0; i < Headers.Num(); i++) {
				HttpRequest->SetHeader(Headers[i].Key, Headers[i].Value);
				HeaderLogText.Append(Headers[i].Key + ": " + Headers[i].Value + "\n");
			}
		}

		FString Version = "Emergence " + GetEmergenceVersionNumber() + " EVMOnline";
		HttpRequest->SetHeader("User-Agent", FPlatformHttp::GetDefaultUserAgent() + " " + Version);

		if (Content.Len() > 0 && HttpRequest->GetHeader("Content-Type").Len() > 0) {
			HttpRequest->SetContentAsString(Content);
		}

		if (ProcessRequestInstantly) {
			UE_LOG(LogEmergenceHttp, Display, TEXT("Sent %s request to \"%s\", timing out in %f %s \n%s"), *Verb, *FinalURL, Timeout, *HeaderLogText, *Content);
			HttpRequest->ProcessRequest();
		}
		else {
			UE_LOG(LogEmergenceHttp, Display, TEXT("%s request ready to send to \"%s\", will time out in %f %s \n%s"), *Verb, *FinalURL, Timeout, *HeaderLogText, *Content);
		}

		return HttpRequest;
	};

	//tries to parse a response as json, calls GetResponseErrors automagically
	static FJsonObject TryParseResponseAsJson(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, EErrorCode& ReturnResponseCode) {

		EErrorCode ResponseCode = UHttpHelperLibrary::GetResponseErrors(HttpRequest, HttpResponse, bSucceeded);
		if (!EHttpResponseCodes::IsOk(UEmergenceErrorCode::Conv_ErrorCodeToInt(ResponseCode))) {
			ReturnResponseCode = ResponseCode;
			return FJsonObject();
		}


		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		TSharedRef <TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			if (JsonObject->HasField("statusCode"))
			{
				ReturnResponseCode = UEmergenceErrorCode::Conv_IntToErrorCode(JsonObject->GetIntegerField("statusCode"));
			}
			else {
				//this fixes weird behaviour with GetPersonas
				ReturnResponseCode = EErrorCode::EmergenceOk;
			}
			return *JsonObject.Get();
		}
		ReturnResponseCode = EErrorCode::EmergenceClientJsonParseFailed;
		return FJsonObject();
	};

	//Gets any pre-content parse errors
	static EErrorCode GetResponseErrors(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
		if (!HttpResponse) {
			return EErrorCode::EmergenceClientRequestCancelled;
		}

		//if the elapsed time is greater than the timeout, we've hit the timeout
		if (HttpRequest->GetTimeout().IsSet() && HttpRequest->GetElapsedTime() > HttpRequest->GetTimeout().GetValue()) {
			return EErrorCode::EmergenceClientRequestTimeout;
		}

		//if we didn't succeed, and we are less than the timeout, and the response code is 0
		//it was probably a cancelled request
		if (!bSucceeded &&
			(HttpRequest->GetElapsedTime() < HttpRequest->GetTimeout().GetValue()) &&
			HttpRequest->GetStatus() == EHttpRequestStatus::Failed &&
			HttpResponse->GetResponseCode() == 0) {
			return EErrorCode::EmergenceClientRequestCancelled;
		}

		//If we didn't even get a http response, give failed
		if (!bSucceeded) return EErrorCode::EmergenceClientFailed;

		//if we got one but its not readable, give invalid response
		if (!HttpResponse.IsValid()) return EErrorCode::EmergenceClientInvalidResponse;

		//if we got a readable one but it has a http error, give that
		if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode())) {
			UE_LOG(LogEmergenceHttp, Warning, TEXT("%s"), *HttpResponse->GetContentAsString());
		}
		return UEmergenceErrorCode::Conv_IntToErrorCode(HttpResponse->GetResponseCode());
	};
};
