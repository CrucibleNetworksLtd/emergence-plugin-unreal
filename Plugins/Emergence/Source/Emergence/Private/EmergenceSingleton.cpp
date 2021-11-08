
#include "EmergenceSingleton.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ObjectMacros.h"
#include "GameDelegates.h"

//for HTTP services
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Dom/JsonObject.h"

#include "Windows/WindowsSystemIncludes.h"

#include "Containers/UnrealString.h"


DEFINE_LOG_CATEGORY(LogEmergenceHttp);

UEmergenceSingleton::UEmergenceSingleton() {
}

TMap<TWeakObjectPtr<UGameInstance>, TWeakObjectPtr<UEmergenceSingleton>> UEmergenceSingleton::GlobalManagers{};


UEmergenceSingleton* UEmergenceSingleton::GetEmergenceManager(const UObject* ContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	if (GameInstance)
	{
		TWeakObjectPtr<UEmergenceSingleton>& Manager = GlobalManagers.FindOrAdd(GameInstance);
		if (!Manager.IsValid())
		{
			Manager = NewObject<UEmergenceSingleton>(GameInstance);
			Manager->SetGameInstance(GameInstance);
			Manager->Init();
		}
		UE_LOG(LogTemp, Warning, TEXT("Text %s"), *Manager->GetFName().ToString());
		return Manager.Get();
	}
	UE_LOG(LogTemp, Warning, TEXT("Text %s"), "No manager avalible, whats going on?");
	return nullptr;
}

UEmergenceSingleton* UEmergenceSingleton::ForceInitialize(const UObject* ContextObject)
{
	return GetEmergenceManager(ContextObject);
}

void UEmergenceSingleton::Init()
{
	FGameDelegates::Get().GetEndPlayMapDelegate().AddUObject(this, &UEmergenceSingleton::Shutdown);
	AddToRoot();
}

void UEmergenceSingleton::Shutdown()
{
	FGameDelegates::Get().GetEndPlayMapDelegate().RemoveAll(this);

	RemoveFromRoot();
	MarkPendingKill();
}

//HTTP Services
void UEmergenceSingleton::GetWalletConnectURI_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	FString ResponseStr, ErrorStr;

	if (bSucceeded && HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			UE_LOG(LogTemp, Display, TEXT("GetWalletConnectURI request complete. url=%s code=%d response=%s"), *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *ResponseStr);
			OnGetWalletConnectURIRequestCompleted.Broadcast(*ResponseStr, EErrorCode::EmergenceOk);
			return;
		}
	}
	OnGetWalletConnectURIRequestCompleted.Broadcast(FString(), UErrorCodeFunctionLibrary::GetResponseErrors(HttpResponse, bSucceeded));
}

void UEmergenceSingleton::GetWalletConnectURI()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UEmergenceSingleton::GetWalletConnectURI_HttpRequestComplete);
	HttpRequest->SetURL(APIBase + "getwalletconnecturi");
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Display, TEXT("GetWalletConnectURI request started."));
}

void UEmergenceSingleton::GetQRCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> ResponseCode = UErrorCodeFunctionLibrary::GetResponseErrors(HttpResponse, bSucceeded);
	if (!EHttpResponseCodes::IsOk(UErrorCodeFunctionLibrary::Conv_ErrorCodeToInt(ResponseCode))) {
		OnGetQRCodeCompleted.Broadcast(nullptr, ResponseCode);
		return;
	}

	TArray<uint8> ResponceBytes = HttpResponse->GetContent();
	UTexture2D* QRCodeTexture;
	if (RawDataToBrush(*(FString(TEXT("QRCODE"))), ResponceBytes, QRCodeTexture)) {
		OnGetQRCodeCompleted.Broadcast(QRCodeTexture, EErrorCode::EmergenceOk);
		return;
	}
	else {
		OnGetQRCodeCompleted.Broadcast(nullptr, EErrorCode::EmergenceClientWrongType);
	}
}

void UEmergenceSingleton::GetQRCode()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UEmergenceSingleton::GetQRCode_HttpRequestComplete);
	HttpRequest->SetURL(APIBase + "qrcode");
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Display, TEXT("GetQRCode request started."));
}

bool UEmergenceSingleton::RawDataToBrush(FName ResourceName, const TArray< uint8 >& InRawData, UTexture2D*& LoadedT2D)
{
	int32 Width;
	int32 Height;

	TArray<uint8> DecodedImage;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(InRawData.GetData(), InRawData.Num()))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			if (!LoadedT2D) return false;

			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();

			LoadedT2D->UpdateResource();
			return true;
		}
	}
	return false;
}

void UEmergenceSingleton::GetHandshake_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString Address;
		if (JsonObject.GetObjectField("message")->TryGetStringField("address", Address)) {
			OnGetHandshakeCompleted.Broadcast(Address, StatusCode);
		}
		else {
			OnGetHandshakeCompleted.Broadcast(Address, EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnGetHandshakeCompleted.Broadcast(FString(), StatusCode);
}

void UEmergenceSingleton::GetHandshake()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UEmergenceSingleton::GetHandshake_HttpRequestComplete);
	HttpRequest->SetURL(APIBase + "handshake");
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Display, TEXT("GetHandshake request started."));
}

void UEmergenceSingleton::GetBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString Balance;
		if (JsonObject.GetObjectField("message")->TryGetStringField("balance", Balance)) {
			OnGetBalanceCompleted.Broadcast(Balance, StatusCode);
		}
		else {
			OnGetBalanceCompleted.Broadcast(FString(), EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnGetBalanceCompleted.Broadcast(FString(), StatusCode);
}

void UEmergenceSingleton::GetBalance()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UEmergenceSingleton::GetBalance_HttpRequestComplete);
	HttpRequest->SetURL(APIBase + "getbalance");
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Display, TEXT("GetBalance request started."));
}

void UEmergenceSingleton::IsConnected_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	
	if (StatusCode == EErrorCode::EmergenceOk) {
		bool IsConnected;
		FString Address;
		if (JsonObject.GetObjectField("message")->TryGetBoolField("isConnected", IsConnected)) {
			Address = JsonObject.GetObjectField("message")->GetStringField("address");
			OnIsConnectedCompleted.Broadcast(IsConnected, Address, StatusCode);
		}
		else {
			OnIsConnectedCompleted.Broadcast(false, FString(), EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnIsConnectedCompleted.Broadcast(false, FString(), StatusCode);
}

void UEmergenceSingleton::IsConnected()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UEmergenceSingleton::IsConnected_HttpRequestComplete);
	HttpRequest->SetURL(APIBase + "isConnected");
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Display, TEXT("IsConnected request started."));
}

void UEmergenceSingleton::KillSession_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TEnumAsByte<EErrorCode> StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		bool Disconnected;
		if (JsonObject.GetObjectField("message")->TryGetBoolField("disconnected", Disconnected)) {
			OnKillSessionCompleted.Broadcast(Disconnected, StatusCode);
		}
		else {
			OnKillSessionCompleted.Broadcast(Disconnected, EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnKillSessionCompleted.Broadcast(false, StatusCode);
}

void UEmergenceSingleton::KillSession()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UEmergenceSingleton::KillSession_HttpRequestComplete);
	HttpRequest->SetURL(APIBase + "killSession");
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Display, TEXT("KillSession request started."));
}

void UEmergenceSingleton::LaunchLocalServerProcess()
{
	FString EmergenceServerBinariesPath = FString(*FPlatformProcess::BaseDir() + "walletConnectpoc.exe");
	FString EmergenceServerPluginPath = FString(FPaths::ProjectPluginsDir() + "Emergence/EmergenceServer/walletConnectpoc.exe");
	FString LoadPath;
	if (FPaths::FileExists(EmergenceServerBinariesPath)) {
		LoadPath = EmergenceServerBinariesPath;
	}
	else if (FPaths::FileExists(EmergenceServerPluginPath)) {
		LoadPath = EmergenceServerPluginPath;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Couldn't find EmergenceServer in binaries or plugin path locations. Make sure you have the server files copied to Plugins/Emergence/EmergenceServer/walletConnectpoc.exe"));
		return;
	}

	if (GConfig) {
		FString EmergenceCustomServerLocation;
		if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("CustomEmergenceServerLocation"), EmergenceCustomServerLocation, GEditorPerProjectIni)) {
			FParse::Value(*EmergenceCustomServerLocation, TEXT("FilePath="), EmergenceCustomServerLocation);
			if (FPaths::FileExists(*EmergenceCustomServerLocation)) {
				LoadPath = EmergenceCustomServerLocation;
				UE_LOG(LogTemp, Warning, TEXT("Found EmergenceServer at override path (%s)."), *EmergenceCustomServerLocation);
			}
		}
	}
	
	UE_LOG(LogTemp, Display, TEXT("Loading Emergence Server from path: %s"), *LoadPath);
	
	const FString JsonArgs("\"{\\\"Name\\\":\\\"Crucibletest\\\",\\\"Description\\\":\\\"UnrealEngineWalletConnect\\\",\\\"Icons\\\":\\\"https:\\/\\/crucible.network\\/wp-content\\/uploads\\/2020\\/10\\/cropped-crucible_favicon-32x32.png\\\",\\\"URL\\\":\\\"https:\\/\\/crucible.network\\\"}\"");

	//Add the args
	TArray<FString> Args = {
		JsonArgs,
		FString::FromInt(FWindowsPlatformProcess::GetCurrentProcessId())
	};

	//combine the args
	FString ArgString = "";
	for (int i = 0; i < Args.Num(); i++) {
		if (i != 0) { //add a space before the next arg
			ArgString = ArgString + " ";
		}
		UE_LOG(LogTemp, Display, TEXT("calling argument [%d]: %s"), i, *Args[i]);
		ArgString = ArgString + Args[i];
	}
	UE_LOG(LogTemp, Display, TEXT("Total argument lenth is %d"), ArgString.Len());
	//create the process
	FPlatformProcess::CreateProc(*LoadPath, *ArgString, false, false, false, nullptr, 0, nullptr, nullptr);
	UE_LOG(LogTemp, Display, TEXT("calling: %s %s"), *LoadPath, *ArgString);
}

void UEmergenceSingleton::KillLocalServerProcess()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UEmergenceSingleton::KillSession_HttpRequestComplete);
	HttpRequest->SetURL(APIBase + "finish");
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Display, TEXT("KillLocalServerProcess request started. Nothing is returned by this."));
}