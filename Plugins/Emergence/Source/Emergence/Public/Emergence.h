// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "EmergenceDeployment.h"
#include "WalletService/WriteMethod.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEmergence, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEmergenceHttp, Log, All);

class UWriteMethod;
struct EmergenceLocalEVMJSON;

struct RustExampleLibraryFunctionReturnStruct {
	bool active;
	char* username;
	uint64 sign_in_count;
};

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

	typedef RustExampleLibraryFunctionReturnStruct(*_getRustExampleLibraryFunction)(bool active, char* username, uint64 sign_in_count, char* returnedstring, bool* returnedbool, uint64* returnedint);
	_getRustExampleLibraryFunction RustExampleLibraryFunction;
};
