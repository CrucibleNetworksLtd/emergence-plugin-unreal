// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "GetWalletConnectURI.generated.h"

UENUM(BlueprintType)
enum class EWalletConnectState : uint8
{
	WaitingForConnection UMETA(ToolTip = "We're waiting for the user to connect their wallet"),
	ConnectionSuccessful UMETA(ToolTip = "The user has successfully connected their wallet"),
	ConnectionTimedout UMETA(ToolTip = "The user has failed to connect their wallet, and WalletConnect timed out"),
	ConnectionRefused UMETA(ToolTip = "The user rejected the connection")
};

UCLASS()
class EMERGENCELOCALWALLETCONNECT_API UGetWalletConnectURI : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UGetWalletConnectURI* GetWalletConnectURI(UObject* WorldContextObject);

	virtual void Activate() override;

	void UpdateStatus();

	void Timeout();

	UTF16CHAR* uriBuffer;
	int status;

	FTimerHandle RepeatingTimerHandle;

	FTimerHandle TimeoutTimerHandle;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWalletConnectStateChanged, FString, WalletConnectURL, EWalletConnectState, State);

	UPROPERTY(BlueprintAssignable)
	FWalletConnectStateChanged WalletConnectStateChanged;
private:
	UPROPERTY()
	bool HasReportedWalletConnectURI = false;
};
