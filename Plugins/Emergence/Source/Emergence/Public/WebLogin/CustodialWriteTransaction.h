// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "CustodialWriteTransaction.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UCustodialWriteTransaction : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	//@TODO add states

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "Content"), Category = "Emergence|Custodial Login")
	static UCustodialWriteTransaction* CustodialWriteTransaction(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract, FString Method, FString Value, TArray<FString> Content);

	void GetEncodedPayload_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetEncodedData_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	bool HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete);

	void SendTransaction_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustodialWriteTransactionCompleted, const FString, TransactionHash, EErrorCode, StatusCode);

	TUniquePtr<FHttpServerResponse> GetHttpPage();

	UPROPERTY(BlueprintAssignable)
	FOnCustodialWriteTransactionCompleted OnCustodialWriteTransactionCompleted;

	UPROPERTY()
	UEmergenceDeployment* DeployedContract;

	UPROPERTY()
	FString Method;
	
	UPROPERTY()
	FString Value;
	
	UPROPERTY()
	TArray<FString> Content;
		
	UPROPERTY()
	FString EncodedSig;

	UPROPERTY()
	FString TransactionNonce;

	UPROPERTY()
	FString UnsignedTransaction;

	static FJsonObject RawTransactionWithoutSignature;
	static FString RpcUrl;

	static bool TransactionInProgress;

	void EncodeTransaction(FString Eoa, FString ChainId, FString ToAddress, FString Value, FString Data, FString RpcUrl);

	void CleanupHttpRoute();

	void GetEncodedData();

	void TransactionEnded();

	void Activate() override;
	void BeginDestroy() override;
	static FHttpRouteHandle RouteHandle;
	static UObject* ContextObject;
	static bool _isServerStarted;
};
