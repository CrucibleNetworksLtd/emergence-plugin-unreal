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
#include "Types/EmergenceErrorCode.h"
#include "GameFramework/PlayerController.h"
#include "Emergence.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Types/Environment.h"
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

	//Get the global Emergence service
	UFUNCTION(BlueprintPure, Category = "Emergence", meta = (DisplayName = "Get Emergence Service", WorldContext = "ContextObject", CompactNodeTitle = "Emergence"))
	static UEmergenceSingleton* GetEmergenceManager(const UObject* ContextObject);

	/** Force initialize the emergence manager, this shouldn't be nessacery. Just a version of GetEmergenceManager with an execute input.  */
	UFUNCTION()
	void CompleteLoginViaWebLoginFlow(const FEmergenceCustodialLoginOutput LoginData, EErrorCode ErrorCode);

	//Are we logged in via a web login flow, rather than WalletConnect? 
	//This is referenced by functions such as RequestToSign and WriteMethod to allow for behaviour switching depending on login type.
	//You shouldn't need to ever change this manually. If you do, what caused you to need to change it manually is a bug and should be reported to Crucible.
	UPROPERTY(BlueprintReadOnly, Category = "Emergence|Login Flow")
	bool UsingWebLoginFlow = false;

	UPROPERTY()
	FString DeviceID;

	//Stores a list of contracts with loaded ABIs, in the format blockchain name + address. 
	//This is used by various methods to check if the ABI needs to be sent to the server (if it is "new" to the server for this user)
	UPROPERTY()
	TArray<FString> ContractsWithLoadedABIs;

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

	//GetQRCode stuff
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void GetQRCode();

	UFUNCTION()
	static bool RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData, UTexture2D*& LoadedT2D);

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

	//Kills the walletconnect session. Setting TrackRequest to false will mean OnKillSessionCompleted will never fire,
	//and this request won't be added to ActiveRequests (good to prevent this getting premptively killed going from PIE back to Editor.
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Overlay Methods")
	void KillSession(bool TrackRequest = true);

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