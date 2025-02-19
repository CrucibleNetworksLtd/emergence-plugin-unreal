// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HttpFwd.h"
#include "VirtualsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ELIZA_API UVirtualsLibrary : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void SendStreamRequest();

	bool OnDataReceived(void* Ptr, int64 Length);

	void GetStream_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void SendVirtualsMessage(UObject* WorldContextObject, FString Message);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	void ChangeVirtualsLocation(UObject* WorldContextObject, FString Location, FString Atmosphere);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOnVirtualMessageRecieved, FString, Message);

	UPROPERTY(BlueprintAssignable)
	FOnOnVirtualMessageRecieved OnVirtualMessageRecieved;

	void SendVirtualsMessage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
};
