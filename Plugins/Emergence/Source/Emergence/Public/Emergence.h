// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Types/EmergenceDeployment.h"
#include "WalletService/WriteMethod.h"

EMERGENCE_API DECLARE_LOG_CATEGORY_EXTERN(LogEmergence, Log, All);
EMERGENCE_API DECLARE_LOG_CATEGORY_EXTERN(LogEmergenceHttp, Log, All);

class UWriteMethod;
struct EmergenceLocalEVMJSON;

class FEmergenceModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void SendTransactionViaKeystore(UWriteMethod* WriteMethod, UEmergenceDeployment* Deployment, FString MethodName, FString PrivateKey, FString PublicKey, FString GasPrice, FString Value, FString& TransactionResponse);
private:
	/** Handle to the test dll we will load */
	void* ExampleLibraryHandle;

	typedef int(*_getExampleLibraryFunction)(wchar_t* fullpath, int length, EmergenceLocalEVMJSON* Data);
	_getExampleLibraryFunction ExampleLibraryFunction;
};
