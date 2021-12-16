// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "GetAvatar.generated.h"

/**

 */
UCLASS()
class EMERGENCE_API UGetAvatar : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	
	//For getting ERC721 avatars
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|ERC721 Helpers")
	static UGetAvatar* GetAvatar(const UObject* WorldContextObject, FString AvatarMetadataURI);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetAvatarCompleted, UTexture2D*, Avatar, FString, Name, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetAvatarCompleted OnGetAvatarCompleted;
private:
	void GetAvatar_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UFUNCTION()
	void AvatarReturned(UTexture2D* Texture, TEnumAsByte<EErrorCode> StatusCode);
	const UObject* WorldContextObject;
	FString AvatarMetadataURI;
	FString ERC721Name;
	int TokenId;
};
