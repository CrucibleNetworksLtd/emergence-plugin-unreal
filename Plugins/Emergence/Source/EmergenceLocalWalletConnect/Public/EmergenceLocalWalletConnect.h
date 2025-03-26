// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FEmergenceLocalWalletConnectModule : public IModuleInterface
{
public:
	void LoadLibrary();

	void FreeLibrary();

private:
	//Handle to Agu's new implementation of a local EVM client
	void* LocalEVMLibraryHandle;

public:
	typedef void(*_GetURIHandle)(UTF16CHAR& uri, int& status);
	static _GetURIHandle GetURI;

	typedef char* (*_getRequestToSignHandle)(char* messagePtr);
	static _getRequestToSignHandle RequestToSignHandle;
};
