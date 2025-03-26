// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "LocalRequestToSign.generated.h"

UCLASS()
class EMERGENCELOCALWALLETCONNECT_API ULocalRequestToSign : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static ULocalRequestToSign* LocalRequestToSign(UObject* WorldContextObject, FString MessageToSign);

	virtual void Activate() override;

	UTF16CHAR* uriBuffer;
	int status;

	FTimerHandle RepeatingTimerHandle;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLocalRequestToSignCompleted, FString, SignedMessage, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnLocalRequestToSignCompleted OnLocalRequestToSignCompleted;
private:
	FString MessageToSign;
};
