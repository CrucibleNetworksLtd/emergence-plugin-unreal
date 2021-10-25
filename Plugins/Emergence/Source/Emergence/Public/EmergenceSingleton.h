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
#include "ErrorCodeFunctionLibrary.h"
#include "EmergenceSingleton.generated.h"
#pragma warning( push )
#pragma warning( disable : 4996 )

UCLASS()
class EMERGENCE_API UEmergenceSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UEmergenceSingleton();

	const FString APIBase = TEXT("http://localhost:50733/api/");

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

	void GetHandshake_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void IsConnected_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void KillSession_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
public:

	//GetWalletConnectURI stuff
	UFUNCTION(BlueprintCallable)
	void GetWalletConnectURI();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetWalletConnectURIRequestCompleted, FString, WalletConnectURI, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnGetWalletConnectURIRequestCompleted OnGetWalletConnectURIRequestCompleted;


	//GetQRCode stuff
	UFUNCTION(BlueprintCallable)
	void GetQRCode();

	bool RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData, UTexture2D*& LoadedT2D);

	static TSharedPtr<FSlateDynamicImageBrush> RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetQRCodeCompleted, UTexture2D*, Icon, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnGetQRCodeCompleted OnGetQRCodeCompleted;

	//Handshake stuff
	UFUNCTION(BlueprintCallable)
	void GetHandshake();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetHandshakeCompleted, FString, Address, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnGetHandshakeCompleted OnGetHandshakeCompleted;

	//Getbalance stuff
	UFUNCTION(BlueprintCallable)
	void GetBalance();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetBalanceCompleted, FString, Balance, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnGetBalanceCompleted OnGetBalanceCompleted;

	//isConnected stuff
	UFUNCTION(BlueprintCallable)
	void IsConnected();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIsConnectedCompleted, bool, IsConnected, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnIsConnectedCompleted OnIsConnectedCompleted;

	//killSession stuff
	UFUNCTION(BlueprintCallable)
	void KillSession();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillSessionCompleted, bool, Response, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnKillSessionCompleted OnKillSessionCompleted;

	// Launch local server process stuff
	UFUNCTION(BlueprintCallable)
	void LaunchLocalServerProcess();

	UFUNCTION(BlueprintCallable)
	void KillLocalServerProcess();

private:
	static TMap<TWeakObjectPtr<UGameInstance>, TWeakObjectPtr<UEmergenceSingleton>> GlobalManagers;
	TWeakObjectPtr<UGameInstance> OwningGameInstance;

	// Launch local server process stuff
	FProcHandle handle;
};
#pragma warning( pop )

DECLARE_LOG_CATEGORY_EXTERN(LogEmergenceHttp, Log, All);