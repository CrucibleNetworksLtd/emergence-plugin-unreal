// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/GameInstance.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Containers/Map.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Containers/Queue.h"
#include "EmergenceSingleton.generated.h"


UCLASS()
class EMERGENCE_API UEmergenceSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UEmergenceSingleton();

	/** Get the global Emergence manager */
	UFUNCTION(BlueprintPure, Category = EmergenceSingleton, meta = (WorldContext = "ContextObject", CompactNodeTitle = "Emergence"))
	static UEmergenceSingleton* GetEmergenceManager(const UObject* ContextObject);

	/** Force initialize the emergence manager, this shouldn't be nessacery. Just a version of GetEmergenceManager with an execute input.  */
	UFUNCTION(BlueprintCallable, Category = EmergenceSingleton, meta = (WorldContext = "ContextObject"))
	static UEmergenceSingleton* ForceInitialize(const UObject* ContextObject);

	void Init();
	void Shutdown();

	void SetGameInstance(UGameInstance* GameInstance) { OwningGameInstance = GameInstance; }


	//HTTPService Functions
private:
	void GetWalletConnectURI_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetQRCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
public:

	//GetWalletConnectURI stuff
	UFUNCTION(BlueprintCallable)
	void GetWalletConnectURI();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetWalletConnectURIRequestCompleted, FString, WalletConnectURI, bool, Succeeded);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnGetWalletConnectURIRequestCompleted OnGetWalletConnectURIRequestCompleted;


	//GetQRCode stuff
	UFUNCTION(BlueprintCallable)
	void GetQRCode();

	bool RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData, UTexture2D*& LoadedT2D);

	static TSharedPtr<FSlateDynamicImageBrush> RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetQRCodeCompleted, UTexture2D*, Icon, bool, Succeeded);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnGetQRCodeCompleted OnGetQRCodeCompleted;

private:
	static TMap<TWeakObjectPtr<UGameInstance>, TWeakObjectPtr<UEmergenceSingleton>> GlobalManagers;
	TWeakObjectPtr<UGameInstance> OwningGameInstance;	
};
