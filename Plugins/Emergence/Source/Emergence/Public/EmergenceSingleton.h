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
#include "PersonaStructs.h"
#include "EmergenceSingleton.generated.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

UCLASS()
class EMERGENCE_API UEmergenceSingleton : public UObject
{
	GENERATED_BODY()
	
public:
	UEmergenceSingleton();

	/** Get the global Emergence manager */
	UFUNCTION(BlueprintPure, Category = "Emergence|EmergenceSingleton", meta = (WorldContext = "ContextObject", CompactNodeTitle = "Emergence"))
	static UEmergenceSingleton* GetEmergenceManager(const UObject* ContextObject);

	/** Force initialize the emergence manager, this shouldn't be nessacery. Just a version of GetEmergenceManager with an execute input.  */
	UFUNCTION(BlueprintCallable, Category = "Emergence|EmergenceSingleton", meta = (WorldContext = "ContextObject"))
	static UEmergenceSingleton* ForceInitialize(const UObject* ContextObject);

	void Init();
	void Shutdown();

	void SetGameInstance(UGameInstance* GameInstance) { OwningGameInstance = GameInstance; }


	//HTTPService Functions
private:
	FString CurrentAccessToken;

	//Returns true if this error code is a 401, and calls OnDatabaseAuthFailed. false on success.
	bool HandleDatabaseServerAuthFail(TEnumAsByte<EErrorCode> ErrorCode);

	void GetWalletConnectURI_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetQRCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetHandshake_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void IsConnected_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void KillSession_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetAccessToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetAccessToken();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDatabaseAuthFailed);
	FOnDatabaseAuthFailed OnDatabaseAuthFailed;

public:
	//Intentionally not exposed to blueprints
	UFUNCTION()
	FString GetCurrentAccessToken();

	//GetWalletConnectURI stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Requests")
	void GetWalletConnectURI();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetWalletConnectURIRequestCompleted, FString, WalletConnectURI, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnGetWalletConnectURIRequestCompleted OnGetWalletConnectURIRequestCompleted;


	//GetQRCode stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Requests")
	void GetQRCode();

	bool RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData, UTexture2D*& LoadedT2D);

	static TSharedPtr<FSlateDynamicImageBrush> RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetQRCodeCompleted, UTexture2D*, Icon, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnGetQRCodeCompleted OnGetQRCodeCompleted;

	//Handshake stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Requests")
	void GetHandshake();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetHandshakeCompleted, FString, Address, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnGetHandshakeCompleted OnGetHandshakeCompleted;

	//Getbalance stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Requests")
	void GetBalance();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetBalanceCompleted, FString, Balance, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnGetBalanceCompleted OnGetBalanceCompleted;

	//isConnected stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Requests")
	void IsConnected();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnIsConnectedCompleted, bool, IsConnected, FString, Address, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnIsConnectedCompleted OnIsConnectedCompleted;

	//killSession stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence|Emergence Requests")
	void KillSession();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillSessionCompleted, bool, Response, TEnumAsByte<EErrorCode>, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnKillSessionCompleted OnKillSessionCompleted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetAccessTokenCompleted, TEnumAsByte<EErrorCode>, StatusCode);
	FOnGetAccessTokenCompleted OnGetAccessTokenCompleted;

private:
	static TMap<TWeakObjectPtr<UGameInstance>, TWeakObjectPtr<UEmergenceSingleton>> GlobalManagers;
	TWeakObjectPtr<UGameInstance> OwningGameInstance;
};
#pragma warning( pop )

DECLARE_LOG_CATEGORY_EXTERN(LogEmergenceHttp, Log, All);