// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Interfaces/IHttpRequest.h"
#include "InteroperableAsset.h"
#include "GetAvatarByNftId.generated.h"


UCLASS()
class EMERGENCE_API UGetAvatarByNftId : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the avatar of a given ID from the Avatar System.
	 * @param AvatarIdString AvatarIdString of the avatar we want to get the details of.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Interoperable Asset Service")
	static UGetAvatarByNftId* GetAvatarByNftId(UObject* WorldContextObject, const FString& AvatarIdString);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetAvatarByNftIdCompleted, FEmergenceInteroperableAsset, Avatar, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetAvatarByNftIdCompleted OnGetAvatarByNftIdCompleted;

private:
	void GetAvatarByNftId_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString AvatarIdString;
};
