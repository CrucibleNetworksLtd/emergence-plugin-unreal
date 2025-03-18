// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "GetWalletConnectURI.generated.h"

UCLASS()
class EMERGENCEBLOCKCHAINWALLET_API UGetWalletConnectURI : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UGetWalletConnectURI* GetWalletConnectURI(UObject* WorldContextObject);

	virtual void Activate() override;

	void UpdateStatus();

	UTF16CHAR* uriBuffer;
	int status;

	FTimerHandle RepeatingTimerHandle;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetWalletConnectURICompleted, FString, Balance, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetWalletConnectURICompleted OnGetWalletConnectURICompleted;
private:
};
