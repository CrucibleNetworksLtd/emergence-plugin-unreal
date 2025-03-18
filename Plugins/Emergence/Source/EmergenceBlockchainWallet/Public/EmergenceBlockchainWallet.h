// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Types/EmergenceDeployment.h"
#include "WalletService/WriteMethod.h"

class UWriteMethod;
struct EmergenceLocalEVMJSON;

class FEmergenceBlockchainWalletModule : public IModuleInterface
{
public:
	void StartupModule() override;

	void SendTransactionViaKeystore(UWriteMethod* WriteMethod, UEmergenceDeployment* Deployment, FString MethodName, FString PrivateKey, FString PublicKey, FString GasPrice, FString Value, FString& TransactionResponse);
private:
	/** Handle to the test dll we will load */
	void* ContainerLibraryHandle;

	//Handle to the function that the container library contains
	typedef int(*_getSendTransactionViaKeystoreFunctionHandle)(wchar_t* fullpath, int length, EmergenceLocalEVMJSON* Data);
	_getSendTransactionViaKeystoreFunctionHandle SendTransactionViaKeystoreFunctionHandle;

	//Handle to Agu's new implementation of a local EVM client
	void* LocalEVMLibraryHandle;

public:
	typedef void(*_GetURIHandle)(UTF16CHAR& uri, int& status);

	static _GetURIHandle GetURI;


};
