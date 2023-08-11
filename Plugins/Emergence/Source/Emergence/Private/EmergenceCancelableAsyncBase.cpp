// Fill out your copyright notice in the Description page of Project Settings.


#include "EmergenceCancelableAsyncBase.h"

void UEmergenceCancelableAsyncBase::Cancel()
{
}

bool UEmergenceCancelableAsyncBase::IsActive() const
{
	return true;
}

bool UEmergenceCancelableAsyncBase::ShouldBroadcastDelegates() const
{
	return true;
}

