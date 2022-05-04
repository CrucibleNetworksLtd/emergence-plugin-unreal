// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "GetTextureFromUrl.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Dom/JsonObject.h"
#include "EmergenceSingleton.h"

void UGetTextureFromUrl::Activate()
{
	if (AllowCacheUsage) {
		CachedTexturePtr = UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->DownloadedImageCache.Find(this->Url);
		if (CachedTexturePtr) {
			//even though we have this ready, we need to send it with a tiny timer or the node won't make sense (it will return its completed before its regular node flow has completed, THAT WOULD BE REALLY REALLY BAD AND CONFUSING).
			this->Timer = WorldContextObject->GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UGetTextureFromUrl::WaitOneFrame);
			return;
		}
	}

	UHttpHelperLibrary::ExecuteHttpRequest<UGetTextureFromUrl>(this, &UGetTextureFromUrl::GetTextureFromUrl_HttpRequestComplete, this->Url);
	if (AllowCacheUsage) {
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetTextureFromUrl request started (%s), didn't find it in the cache, calling GetTextureFromUrl_HttpRequestComplete on request completed"), *this->Url);
	}
	else {
		UE_LOG(LogEmergenceHttp, Display, TEXT("GetTextureFromUrl request started (%s), calling GetTextureFromUrl_HttpRequestComplete on request completed"), *this->Url);
	}
}

void UGetTextureFromUrl::GetTextureFromUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode ResponseCode = UErrorCodeFunctionLibrary::GetResponseErrors(HttpResponse, bSucceeded);
	if (!EHttpResponseCodes::IsOk(UErrorCodeFunctionLibrary::Conv_ErrorCodeToInt(ResponseCode))) {
		OnGetTextureFromUrlCompleted.Broadcast(nullptr, ResponseCode);
		return;
	}

	TArray<uint8> ResponceBytes = HttpResponse->GetContent();
	UTexture2D* QRCodeTexture;
	if (RawDataToBrush(*(FString(TEXT("QRCODE"))), ResponceBytes, QRCodeTexture)) {
		OnGetTextureFromUrlCompleted.Broadcast(QRCodeTexture, EErrorCode::EmergenceOk);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->DownloadedImageCache.Add(this->Url, QRCodeTexture);
		return;
	}
	else {
		OnGetTextureFromUrlCompleted.Broadcast(nullptr, EErrorCode::EmergenceClientWrongType);
	}
}

bool UGetTextureFromUrl::RawDataToBrush(FName ResourceName, const TArray< uint8 >& InRawData, UTexture2D*& LoadedT2D)
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

void UGetTextureFromUrl::WaitOneFrame()
{
	WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(Timer);
	OnGetTextureFromUrlCompleted.Broadcast(*CachedTexturePtr, EErrorCode::EmergenceOk);
	UE_LOG(LogEmergenceHttp, Display, TEXT("GetTextureFromUrl request started (%s), found it in cache, returning"), *this->Url);
	Timer.Invalidate();
}
