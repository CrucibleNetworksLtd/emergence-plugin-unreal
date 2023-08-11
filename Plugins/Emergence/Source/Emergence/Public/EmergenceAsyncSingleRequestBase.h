// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceCancelableAsyncBase.h"
#include "Interfaces/IHttpRequest.h"
#include "EmergenceAsyncSingleRequestBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class EMERGENCE_API UEmergenceAsyncSingleRequestBase : public UEmergenceCancelableAsyncBase
{
	GENERATED_BODY()
public:

	virtual void Cancel();

	virtual bool IsActive() const;

	virtual bool ShouldBroadcastDelegates() const;

	FHttpRequestPtr Request;

private:
	UPROPERTY();
	bool IsCancelled = false; 
};
