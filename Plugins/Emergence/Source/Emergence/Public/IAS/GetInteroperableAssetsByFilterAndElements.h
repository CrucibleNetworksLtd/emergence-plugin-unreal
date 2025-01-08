// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Interfaces/IHttpRequest.h"
#include "InteroperableAsset.h"
#include "GetInteroperableAssetsByFilterAndElements.generated.h"

UCLASS()
class EMERGENCE_API UGetInteroperableAssetsByFilterAndElements : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * 
	 * @param AvatarIdString 
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "NFTIDs"), Category = "Emergence|Interoperable Asset Service")
	static UGetInteroperableAssetsByFilterAndElements* GetInteroperableAssetsByFilterAndElements(UObject* WorldContextObject, const TSet<TSubclassOf<UEmergenceInteroperableAssetElement>>& DesiredElements, const FString& CollectionID, const FString& WalletAddress, const TArray<FString>& NFTIDs);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetInteroperableAssetsByFilterAndElementsCompleted, const TArray<FEmergenceInteroperableAsset>&, InteroperableAssets, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetInteroperableAssetsByFilterAndElementsCompleted OnGetInteroperableAssetsByFilterAndElementsCompleted;

private:
	void OnGetInteroperableAssetsByFilterAndElements_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString CollectionID, WalletAddress;
	TArray<FString> NFTIDs;
	TSet<TSubclassOf<UEmergenceInteroperableAssetElement>> DesiredElements;
};
