
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
			OnGetWalletConnectURIRequestCompleted.Broadcast(*ResponseStr, true);
			return;
		}
	}
	OnGetWalletConnectURIRequestCompleted.Broadcast(FString(), false);
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
	FString ResponseStr, ErrorStr;

	if (bSucceeded && HttpResponse.IsValid())
	{
		TArray<uint8> ResponceBytes = HttpResponse->GetContent();
		UTexture2D* QRCodeTexture;
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			if (RawDataToBrush(*(FString(TEXT("QRCODE"))), ResponceBytes, QRCodeTexture)) {
				UE_LOG(LogTemp, Display, TEXT("GetQRCode request complete. url=%s code=%d response=%s"), *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *ResponseStr);
				OnGetQRCodeCompleted.Broadcast(QRCodeTexture, true);
				return;
			}	
		}
	}
	OnGetQRCodeCompleted.Broadcast(nullptr, false);
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
	FString ResponseStr, ErrorStr;

	if (bSucceeded && HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
			TSharedRef <TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*ResponseStr);
			int StatusCode = -1;
			FString Address;
			if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
			{
				StatusCode = JsonObject->GetIntegerField("statusCode");
				Address = JsonObject->GetObjectField("message")->GetStringField("address");
			}
			UE_LOG(LogTemp, Display, TEXT("GetHandshake request complete. url=%s code=%d response=%s"), *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *ResponseStr);
			OnGetHandshakeCompleted.Broadcast(*Address, true);
			return;
		}
	}
	OnGetHandshakeCompleted.Broadcast(FString(), false);
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
	FString ResponseStr, ErrorStr;

	if (bSucceeded && HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
			TSharedRef <TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*ResponseStr);
			int StatusCode = -1;
			int Balance = -1;
			if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
			{
				StatusCode = JsonObject->GetIntegerField("statusCode");
				Balance = JsonObject->GetObjectField("message")->GetIntegerField("balance");
			}
			UE_LOG(LogTemp, Display, TEXT("GetBalance request complete. url=%s code=%d response=%s"), *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *ResponseStr);
			OnGetBalanceCompleted.Broadcast(Balance, true);
			return;
		}
	}
	OnGetBalanceCompleted.Broadcast(-1, false);
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
	FString ResponseStr, ErrorStr;

	if (bSucceeded && HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
			TSharedRef <TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*ResponseStr);
			int StatusCode = -1;
			bool IsConnected = false;
			if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
			{
				StatusCode = JsonObject->GetIntegerField("statusCode");
				IsConnected = JsonObject->GetObjectField("message")->GetBoolField("isConnected");
			}
			UE_LOG(LogTemp, Display, TEXT("IsConnected request complete. url=%s code=%d response=%s"), *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *ResponseStr);
			OnIsConnectedCompleted.Broadcast(StatusCode, IsConnected, true);
			return;
		}
	}
	OnIsConnectedCompleted.Broadcast(-1, false, false);
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
	FString ResponseStr, ErrorStr;

	if (bSucceeded && HttpResponse.IsValid())
	{
		ResponseStr = HttpResponse->GetContentAsString();
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		TSharedRef <TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(*ResponseStr);
		int StatusCode = -1;
		bool Disconnected;
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			StatusCode = JsonObject->GetIntegerField("statusCode");
			Disconnected = JsonObject->GetObjectField("message")->GetBoolField("disconnected");
		}
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			UE_LOG(LogTemp, Display, TEXT("KillSession request complete. url=%s code=%d response=%s"), *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *ResponseStr);
			OnKillSessionCompleted.Broadcast(Disconnected, true);
			return;
		}
	}
	OnKillSessionCompleted.Broadcast(false, false);
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