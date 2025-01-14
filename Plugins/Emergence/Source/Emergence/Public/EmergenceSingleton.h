// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

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
#include "GameFramework/PlayerController.h"
#include "Emergence.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Futurepass/GetLinkedFuturepassInformation.h"
#include "Environment.h"
#include "WebLogin/CustodialLogin.h"
#include "WalletService/RequestToSign.h"
#include "EmergenceSingleton.generated.h"

#pragma warning( push )
#pragma warning( disable : 4996 )

UCLASS()
class EMERGENCE_API UEmergenceSingleton : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	template<typename T>
	inline static T StringToEnum(const FString& Name) {
		UEnum* EnumClass = StaticEnum<T>();
		if (!EnumClass) {
			UE_LOG(LogEmergence, Fatal, TEXT("StringToEnum Enum not found: %s"), *Name);
		}
		return (T)EnumClass->GetIndexByName(FName(*Name), EGetByNameFlags::ErrorIfNotFound);
	}

	//Get the global Emergence service
	UFUNCTION(BlueprintPure, Category = "Emergence", meta = (DisplayName = "Get Emergence Service", WorldContext = "ContextObject", CompactNodeTitle = "Emergence"))
	static UEmergenceSingleton* GetEmergenceManager(const UObject* ContextObject);

	/** Force initialize the emergence manager, this shouldn't be nessacery. Just a version of GetEmergenceManager with an execute input.  */
	UFUNCTION()
	void CompleteLoginViaWebLoginFlow(const FEmergenceCustodialLoginOutput LoginData, EErrorCode ErrorCode);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustodialLoginCompleted, FEmergenceCustodialLoginOutput, TokenData, EErrorCode, StatusCode);

	UPROPERTY()
	FOnCustodialLoginCompleted OnWebLoginCompleted;

	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Overlay Methods")
	static EFutureverseEnvironment GetFutureverseEnvironment();

	//Sets the Emergence Singleton's cache of the futurepass information (and sets FuturepassInfoIsSet to true)
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void SetFuturepassInfomationCache(FLinkedFuturepassInformationResponse FuturepassInfo);

	//Clears the Emergence Singleton's cache of the futurepass information (and sets FuturepassInfoIsSet to false)
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void ClearFuturepassInfomationCache();

	//Cache of the last Futurepass information set as part of SetFuturepassInfomationCache
	UPROPERTY(BlueprintReadOnly, Category = "Emergence|Futureverse")
	FLinkedFuturepassInformationResponse FuturepassInfoCache;

	//Is FuturepassInfoCache valid?
	UPROPERTY(BlueprintReadOnly, Category = "Emergence|Futureverse")
	bool FuturepassInfoCacheIsSet = false;

	//Are we logged in via a web login flow, rather than WC
	UPROPERTY(BlueprintReadOnly, Category = "Emergence|Login Flow")
	bool UsingWebLoginFlow = false;

	UPROPERTY()
	FString DeviceID;

	UPROPERTY()
	TArray<FString> ContractsWithLoadedABIs;

	UPROPERTY()
	TMap<FString, UTexture2D*> DownloadedImageCache;

	//HTTPService Functions
private:
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> GetHandshakeRequest;

	UPROPERTY()
	FString CurrentAddress = "";

	UPROPERTY()
	FString CurrentChecksummedAddress = "";

	void GetQRCode_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetHandshake_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void IsConnected_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void KillSession_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void ReinitializeWalletConnect_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
public:
	//Cancels any open GetHandshake requests.
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Emergence Singleton")
	void CancelSignInRequest();

	//Do we have a wallet connected address? This will likely only be true when the player has logged in via wallet connect.
	UFUNCTION(BlueprintPure, Category = "Emergence|Emergence Singleton")
	bool HasCachedAddress();

	//Returns the last wallet connected address (if GetHandshake has been called already) If we don't have one yet, returns "-1".
	UFUNCTION(BlueprintPure, Category = "Emergence|Emergence Singleton")
	FString GetCachedAddress(bool Checksummed = false);

	UFUNCTION()
	FString GetCachedChecksummedAddress();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetWalletConnectURIRequestCompleted, FString, WalletConnectURI, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Emergence Singleton")
	FOnGetWalletConnectURIRequestCompleted OnGetWalletConnectURIRequestCompleted;


	//GetQRCode stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void GetQRCode();

	UFUNCTION()
	static bool RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData, UTexture2D*& LoadedT2D);

	static TSharedPtr<FSlateDynamicImageBrush> RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGetQRCodeCompleted, UTexture2D*, Icon, FString, WalletConnectString, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Emergence Singleton")
	FOnGetQRCodeCompleted OnGetQRCodeCompleted;

	//Handshake stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void GetHandshake(int Timeout = 60);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetHandshakeCompleted, FString, Address, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Emergence Singleton")
	FOnGetHandshakeCompleted OnGetHandshakeCompleted;

	//ReinitializeWalletConnect stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void ReinitializeWalletConnect();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReinitializeWalletConnectCompleted, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Emergence Singleton")
	FOnReinitializeWalletConnectCompleted OnReinitializeWalletConnectCompleted;

	//isConnected stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void IsConnected();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnIsConnectedCompleted, bool, IsConnected, FString, Address, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "Emergence Internal|Emergence Singleton")
	FOnIsConnectedCompleted OnIsConnectedCompleted;

	//killSession stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void KillSession();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillSessionCompleted, bool, Response, EErrorCode, StatusCode);

	//Called when the Emergence session ends and a new WalletConnect / Futureverse custodial wallet connection can be started
	UPROPERTY(BlueprintAssignable, Category = "Emergence|Emergence Singleton")
	FOnKillSessionCompleted OnKillSessionCompleted;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginFinished, EErrorCode, StatusCode);
	
	//Called when the user has done the last step of a login process, or the process has had an error that causes it to finish.
	UPROPERTY(BlueprintAssignable, Category = "Emergence|Emergence Singleton")
	FOnLoginFinished OnLoginFinished;
};
#pragma warning( pop )