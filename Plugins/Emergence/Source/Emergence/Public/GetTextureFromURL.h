// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Emergence.h"
#include "GetTextureFromUrl.generated.h"
/**
 */
UCLASS()
class EMERGENCE_API UGetTextureFromUrl : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	//Takes a URL string and gets the PNG from it. Not tested with anything other than a PNG.
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", DisplayName="Get Texture From URL (PNG)"), Category = "Emergence|Helpers")
	static UGetTextureFromUrl* TextureFromUrl(const FString& Url) {
		UGetTextureFromUrl* BlueprintNode = NewObject<UGetTextureFromUrl>(); //I don't know why, but every time I tried to put this in the cpp file it wouldn't link it properly and would fail to compile. If you think you can fix it, go ahead.
		BlueprintNode->Url = FString(Url);
		return BlueprintNode;
	}

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTextureFromUrlCompleted, UTexture2D*, Texture, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetTextureFromUrlCompleted OnGetTextureFromUrlCompleted;

	FString Url;
private:
	void GetTextureFromUrl_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	static bool RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData, UTexture2D*& LoadedT2D);
	
};
