// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "AvatarByOwner.generated.h"


USTRUCT(BlueprintType)
struct FEmergenceAvatarResult
{

  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString avatarId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString contractAddress;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString tokenId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString tokenURI;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 lastUpdated;

  FEmergenceAvatarResult() {};

  FEmergenceAvatarResult( FString _avatarId, FString _contractAddress, FString _tokenId, FString _tokenURI, int32 _lastUpdated ){

    avatarId = _avatarId;
    contractAddress = _contractAddress;
    tokenId = _tokenId;
    tokenURI = _tokenURI;
    lastUpdated = _lastUpdated;
  
  }
  
  FEmergenceAvatarResult(FString _json_){
    FEmergenceAvatarResult _tmpEmergenceAvatarsResult;
    
		FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceAvatarResult>(
		_json_,
		&_tmpEmergenceAvatarsResult,
    0, 0);
    
    avatarId = _tmpEmergenceAvatarsResult.avatarId;
    contractAddress = _tmpEmergenceAvatarsResult.contractAddress;
    tokenId = _tmpEmergenceAvatarsResult.tokenId;
    tokenURI = _tmpEmergenceAvatarsResult.tokenURI;
    lastUpdated = _tmpEmergenceAvatarsResult.lastUpdated;
  }
  
};

UCLASS()
class EMERGENCE_API UAvatarByOwner : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the all the avatars of a given address from the Avatar System.
	 * @param Address Address to get the avatars of.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Emergence Requests|Avatar Service")
	static UAvatarByOwner* AvatarByOwner(const UObject* WorldContextObject, const FString& Address);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAvatarByOwnerCompleted, const TArray<FEmergenceAvatarResult>&, Avatars, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnAvatarByOwnerCompleted OnAvatarByOwnerCompleted;
private:
	void AvatarByOwner_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString Address;
	const UObject* WorldContextObject;
};
