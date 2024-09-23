// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "CustodialLogin.generated.h"

//This only exists because I can't output a tmap from a dynamic multicast deligate
//@TODO Hide this from blueprint when we're done
USTRUCT(BlueprintType)
struct FEmergenceCustodialLoginOutput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FString> TokenData;

	FEmergenceCustodialLoginOutput() {};

	FEmergenceCustodialLoginOutput(TMap<FString, FString> _TokenData) {
		TokenData = _TokenData;
	};
};

UCLASS()
class EMERGENCE_API UCustodialLogin : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Custodial Login")
	static UCustodialLogin* CustodialLogin(const UObject* WorldContextObject);

	UFUNCTION()
	FString CleanupBase64ForWeb(FString Input);

	TUniquePtr<FHttpServerResponse> GetHttpPage();

	UPROPERTY()
	FString clientid = "8XPY4Vnc6BBn_4XNBYk0P"; //@TODO get an actual client ID

	void Activate() override;
	bool HandleAuthRequestCallback(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	void RequestPrint(const FHttpServerRequest& Request, bool PrintBody = true);

	UFUNCTION()
	FString GetSecureRandomBase64();

	static bool _isServerStarted;

	UPROPERTY()
	const UObject* ContextObject;

private:
	static FString code;

	static FString state;
};
