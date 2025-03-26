// Copyright Crucible Networks Ltd 2025. All Rights Reserved.

#pragma once

#include "Core.h"
#include "HAL/Runnable.h"
#include "EmergenceLocalWalletConnect.h"
#include "LocalEVM/LocalRequestToSign.h"

class EMERGENCELOCALWALLETCONNECT_API FLocalEVMSignMessageThreadRunnable : public FRunnable
{
public:

	FLocalEVMSignMessageThreadRunnable() {};

	//override Init,Run and Stop.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

	char* MessageToSignRef;

	ULocalRequestToSign::FOnLocalRequestToSignCompleted* RequestToSignCompletedHandle;

private:
	FEmergenceLocalWalletConnectModule::_getRequestToSignHandle GetRequestToSignHandle;
};