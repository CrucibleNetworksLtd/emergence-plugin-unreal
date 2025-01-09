// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceSingleton.h"
#include "Engine/Engine.h"
#include "Serialization/JsonWriter.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ObjectMacros.h"
#include "GameDelegates.h"
#include "Engine/Texture2D.h"

//for HTTP services
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Dom/JsonObject.h"

#include "HttpService/HttpHelperLibrary.h"
#include "Containers/UnrealString.h"

#include "EmergenceChainObject.h"

#include "Engine/GameViewportClient.h"
#include "TextureResource.h"

#include "Misc/DateTime.h"

#include "TimerManager.h"


void UEmergenceSingleton::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameInstanceSubsystem::Initialize(Collection);
}

void UEmergenceSingleton::Deinitialize()
{
	UGameInstanceSubsystem::Deinitialize();
}

UEmergenceSingleton* UEmergenceSingleton::GetEmergenceManager(const UObject* ContextObject)
{
	if (!GEngine || !ContextObject) {
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(ContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	if (GameInstance)
	{
		return GameInstance->GetSubsystem<UEmergenceSingleton>();
	}
	UE_LOG(LogEmergenceHttp, Error, TEXT("Emergence singleton error: no game instance"));
	return nullptr;
}


void UEmergenceSingleton::CompleteLoginViaWebLoginFlow(const FEmergenceCustodialLoginOutput LoginData, EErrorCode ErrorCode)
{
	if (ErrorCode == EErrorCode::EmergenceOk) {
		FString Address = LoginData.TokenData.FindRef(TEXT("eoa"));
		this->CurrentChecksummedAddress = Address;
		this->CurrentAddress = Address.ToLower();
		this->UsingWebLoginFlow = true;
		OnLoginFinished.Broadcast(EErrorCode::EmergenceOk);
	}
	else {
		UE_LOG(LogEmergence, Error, TEXT("CompleteLoginViaWebLoginFlow failed with code: %d"), (int)ErrorCode);
	}
}

EFutureverseEnvironment UEmergenceSingleton::GetFutureverseEnvironment()
{

	FString Environment = UHttpHelperLibrary::GetFVEnvironment();

	if (Environment == "Production") {
		//Production Env URL
		return EFutureverseEnvironment::Production;
	}

	if (Environment == "Development") {
		//Development Env URL
		return EFutureverseEnvironment::Development;
	}

	//Staging Env URL
	return EFutureverseEnvironment::Staging;
}

void UEmergenceSingleton::SetFuturepassInfomationCache(FLinkedFuturepassInformationResponse FuturepassInfo)
{
	FuturepassInfoCache = FuturepassInfo;
	FuturepassInfoCacheIsSet = true;
}

void UEmergenceSingleton::ClearFuturepassInfomationCache()
{
	FuturepassInfoCache = FLinkedFuturepassInformationResponse();
	FuturepassInfoCacheIsSet = false;
}

void UEmergenceSingleton::SetOwnedAvatarNFTCache(TArray<FEmergenceAvatarResult> Results)
{
	this->OwnedAvatarNFTCache = Results;
	this->OwnedAvatarNFTCached = true;
	this->OnOwnedAvatarNFTCacheUpdated.Broadcast();
}

void UEmergenceSingleton::FlushOwnedAvatarNFTCache()
{
	this->OwnedAvatarNFTCache.Empty();
	this->OwnedAvatarNFTCached = false;
}

bool UEmergenceSingleton::HandleDatabaseServerAuthFail(EErrorCode ErrorCode)
{
	if (ErrorCode == EErrorCode::Denied) {
		OnDatabaseAuthFailed.Broadcast();
		return true;
	}
	else{
		return false;
	}
}

void UEmergenceSingleton::CancelSignInRequest()
{
	if (GetHandshakeRequest && GetHandshakeRequest->GetStatus() == EHttpRequestStatus::Processing) {
		GetHandshakeRequest->CancelRequest();
	}
}

bool UEmergenceSingleton::HasCachedAddress()
{
	return this->CurrentAddress != FString("");
}

FString UEmergenceSingleton::GetCachedAddress(bool Checksummed)
{
	if (Checksummed) {
		if (this->CurrentChecksummedAddress.Len() > 0) {
			return this->CurrentChecksummedAddress;
		}
		else {
			return FString("-1");
		}
	}
	else {
		if (this->CurrentAddress.Len() > 0) {
			return this->CurrentAddress;
		}
		else {
			return FString("-1");
		}
	}
}

FString UEmergenceSingleton::GetCachedChecksummedAddress()
{
	if (this->CurrentAddress.Len() > 0) {
		return this->CurrentChecksummedAddress;
	}
	else {
		return FString("-1");
	}
}

void UEmergenceSingleton::CallRequestError(FString ConnectionName, EErrorCode StatusCode)
{
	this->OnAnyRequestError.Broadcast(ConnectionName, StatusCode);
}

void UEmergenceSingleton::GetQRCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode ResponseCode = UErrorCodeFunctionLibrary::GetResponseErrors(HttpRequest, HttpResponse, bSucceeded);
	if (ResponseCode != EErrorCode::Ok) { //this endpoint responds Http Ok rather than emergence ok, because its an image rather than emergence status json
		OnGetQRCodeCompleted.Broadcast(nullptr, FString(), ResponseCode);
		OnAnyRequestError.Broadcast("GetQRCode", ResponseCode);
		return;
	}

	TArray<uint8> ResponseBytes = HttpResponse->GetContent();
	UTexture2D* QRCodeTexture;
	if (RawDataToBrush(*(FString(TEXT("QRCODE"))), ResponseBytes, QRCodeTexture)) {

		this->DeviceID = HttpResponse->GetHeader("deviceId");
		FString WalletConnectString = HttpResponse->GetHeader("walletconnecturi");
		OnGetQRCodeCompleted.Broadcast(QRCodeTexture, WalletConnectString, EErrorCode::EmergenceOk);
		return;
	}
	else {
		OnGetQRCodeCompleted.Broadcast(nullptr, FString(), EErrorCode::EmergenceClientFailed);
		OnAnyRequestError.Broadcast("GetQRCode", EErrorCode::EmergenceClientFailed);
	}
}

void UEmergenceSingleton::GetQRCode()
{
	this->DeviceID = ""; //clear the device ID, we'll be getting a new one so we don't want to be able to accidently send an old header

	UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this,&UEmergenceSingleton::GetQRCode_HttpRequestComplete, UHttpHelperLibrary::APIBase + "qrcode");
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetQRCode request started, calling GetQRCode_HttpRequestComplete on request completed"));
}

bool UEmergenceSingleton::RawDataToBrush(FName ResourceName, const TArray< uint8 >& InRawData, UTexture2D*& LoadedT2D)
{
	int32 Width;
	int32 Height;

	TArray<uint8> DecodedImage;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper;

	if (InRawData.Num() == 0) { //if there is no raw data, fail out
		return false;
	}

	if (InRawData[0] == 0x89) {
		ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else if (InRawData[0] == 0xFF && InRawData[1] == 0xD8 && InRawData[2] == 0xFF) {
		ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	else {
		return false;
	}

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(InRawData.GetData(), InRawData.Num()))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			if (!LoadedT2D) return false;

			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();
#if(ENGINE_MINOR_VERSION >= 4) && (ENGINE_MAJOR_VERSION >= 5)
			void* TextureData = LoadedT2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
			LoadedT2D->GetPlatformData()->Mips[0].BulkData.Unlock();
#else
			void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
			LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();
#endif
			LoadedT2D->UpdateResource();
			return true;
		}
	}
	return false;
}

void UEmergenceSingleton::GetHandshake_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		FString Address;
		if (JsonObject.GetObjectField("message")->TryGetStringField("address", Address)) {
			OnGetHandshakeCompleted.Broadcast(Address, StatusCode);
			this->CurrentAddress = Address;
			FString ChecksummedAddress;
			if (JsonObject.GetObjectField("message")->TryGetStringField("checksummedAddress", ChecksummedAddress)) {
				this->CurrentChecksummedAddress = ChecksummedAddress;
			}
			OnLoginFinished.Broadcast(EErrorCode::EmergenceOk);
		}
		else {
			OnGetHandshakeCompleted.Broadcast(Address, EErrorCode::EmergenceClientWrongType);
			OnAnyRequestError.Broadcast("GetHandshake", EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnGetHandshakeCompleted.Broadcast(FString(), StatusCode);
	OnAnyRequestError.Broadcast("GetHandshake", StatusCode);
}

void UEmergenceSingleton::GetHandshake(int Timeout)
{
	TArray<TPair<FString, FString>> Headers;
	if (!this->DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", this->DeviceID));
	}
	Headers.Add(TPair<FString, FString>("timeout", FString::FromInt(Timeout + 2))); //ask for the desired timeout plus buffer time to load the next one
	
	GetHandshakeRequest = UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(
		this,&UEmergenceSingleton::GetHandshake_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "handshake",
		"GET", Timeout, Headers); //use the timeout provided
	
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetHandshake request started, calling GetHandshake_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::ReinitializeWalletConnect_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnReinitializeWalletConnectCompleted.Broadcast(StatusCode);
		return;
	}
	OnReinitializeWalletConnectCompleted.Broadcast(StatusCode);
	OnAnyRequestError.Broadcast("ReinitializeWalletConnect", StatusCode);
}

void UEmergenceSingleton::ReinitializeWalletConnect()
{
	UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this, &UEmergenceSingleton::ReinitializeWalletConnect_HttpRequestComplete, UHttpHelperLibrary::APIBase + "reinitializewalletconnect");
	UE_LOG(LogEmergenceHttp, Display, TEXT("ReinitializeWalletConnect request started, calling ReinitializeWalletConnect_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::IsConnected_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);
	
	if (StatusCode == EErrorCode::EmergenceOk) {
		bool IsConnected;
		FString Address;
		if (JsonObject.GetObjectField("message")->TryGetBoolField("isConnected", IsConnected)) {
			if (IsConnected) {
				Address = JsonObject.GetObjectField("message")->GetStringField("address");
				OnIsConnectedCompleted.Broadcast(IsConnected, Address, StatusCode);
			}
			else {
				OnIsConnectedCompleted.Broadcast(IsConnected, "", StatusCode);
			}
		}
		else {
			OnIsConnectedCompleted.Broadcast(false, FString(), EErrorCode::EmergenceClientWrongType);
			OnAnyRequestError.Broadcast("IsConnected", EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnIsConnectedCompleted.Broadcast(false, FString(), StatusCode);
	OnAnyRequestError.Broadcast("IsConnected", StatusCode);
}

void UEmergenceSingleton::IsConnected()
{
	if (this->DeviceID.IsEmpty()) {
		OnIsConnectedCompleted.Broadcast(false, FString(), EErrorCode::EmergenceOk);
		return;
	}

	TArray<TPair<FString, FString>> Headers;
	if (!this->DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", this->DeviceID));
	}

	UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this,&UEmergenceSingleton::IsConnected_HttpRequestComplete, UHttpHelperLibrary::APIBase + "isConnected", "GET", 60.0F, Headers);
	UE_LOG(LogEmergenceHttp, Display, TEXT("IsConnected request started, calling IsConnected_HttpRequestComplete on request completed"));
}

void UEmergenceSingleton::KillSession_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);
	if (StatusCode == EErrorCode::EmergenceOk) {
		bool Disconnected;
		if (JsonObject.GetObjectField("message")->TryGetBoolField("disconnected", Disconnected)) {
			OnKillSessionCompleted.Broadcast(Disconnected, StatusCode);
			this->DeviceID = "";
		}
		else {
			OnKillSessionCompleted.Broadcast(Disconnected, EErrorCode::EmergenceClientWrongType);
			OnAnyRequestError.Broadcast("KillSession", EErrorCode::EmergenceClientWrongType);
		}
		return;
	}
	OnKillSessionCompleted.Broadcast(false, StatusCode);
	OnAnyRequestError.Broadcast("KillSession", StatusCode);
}

void UEmergenceSingleton::KillSession()
{
	if(this->UsingWebLoginFlow){
		this->CurrentAddress = "";
		this->CurrentChecksummedAddress = "";
		this->DeviceID = "";
		
		OnKillSessionCompleted.Broadcast(true, EErrorCode::EmergenceOk);
	
		return;
	}
	else {

		TArray<TPair<FString, FString>> Headers;

		if (this->DeviceID.IsEmpty()) {
			UE_LOG(LogEmergenceHttp, Display, TEXT("Tried to KillSession but there is no device ID, so probably no session."));
			return;
		}

		//we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", this->DeviceID));

		UHttpHelperLibrary::ExecuteHttpRequest<UEmergenceSingleton>(this, &UEmergenceSingleton::KillSession_HttpRequestComplete, UHttpHelperLibrary::APIBase + "killSession", "GET", 60.0F, Headers);
		UE_LOG(LogEmergenceHttp, Display, TEXT("KillSession request started, calling KillSession_HttpRequestComplete on request completed"));
	}
}

bool UEmergenceSingleton::GetAvatarByGUIDFromCache(FString GUID, FEmergenceAvatarMetadata& FoundAvatar)
{
	for (int i = 0; i < OwnedAvatarNFTCache.Num(); i++) {
		FEmergenceAvatarMetadata* AvatarMetadata = OwnedAvatarNFTCache[i].Avatars.FindByPredicate([&](FEmergenceAvatarMetadata Avatar) {
			return Avatar.GUID == GUID;
			});
		if (AvatarMetadata) {
			FoundAvatar = *AvatarMetadata;
			return true;
		}
	}

	return false;
}
