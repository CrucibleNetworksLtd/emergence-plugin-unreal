// Fill out your copyright notice in the Description page of Project Settings.


#include "EmergenceAsyncSingleRequestBase.h"

bool UEmergenceAsyncSingleRequestBase::IsActive() const
{
	return Request->GetStatus() == EHttpRequestStatus::Processing;
}

void UEmergenceAsyncSingleRequestBase::Cancel() {
	if (Request) {
		Request->OnProcessRequestComplete().Unbind();
		Request->CancelRequest();
	}
	IsCancelled = true;
}

bool UEmergenceAsyncSingleRequestBase::ShouldBroadcastDelegates() const
{
	return !IsCancelled;
}
