// Copyright Crucible Networks Ltd 2025. All Rights Reserved.

#pragma once

#include "Core.h"
#include "HAL/Runnable.h"
#include "EmergenceBlockchainWallet.h"

class EMERGENCEBLOCKCHAINWALLET_API FLocalEVM3ThreadRunnable : public FRunnable
{
public:

	FLocalEVM3ThreadRunnable() {};

	//override Init,Run and Stop.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

	FEmergenceBlockchainWalletModule::_GetURIHandle GetURI;

	UTF16CHAR* UriBufferRef;
	int* StatusRef;
};