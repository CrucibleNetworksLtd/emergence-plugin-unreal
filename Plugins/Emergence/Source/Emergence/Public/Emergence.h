// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "EmergenceDeployment.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEmergence, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogEmergenceHttp, Log, All);

class FEmergenceModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool SendTransactionViaKeystore(UEmergenceDeployment* Deployment, FString MethodName, FString PrivateKey, FString PublicKey, FString GasPrice, FString Value, FString& TransactionResponse);

private:
	/** Handle to the test dll we will load */
	void* ExampleLibraryHandle;

	struct JSON
	{
		int a;
		const char* b;
		const char* _address;
		const char* _ABI;
		const char* _network;
		const char* _nodeUrl;
		const char* _methodName;
		const char* _localAccountName;
		const char* _password;
		const char* _keystorePath;
		const char* _chainID;
		const char* _gasPrice;
		const char* _value;
		const char* _privateKey;
		const char* _publicKey;
		char* result; // to store the result
		int ResultLength;
	};

	typedef int(*_getExampleLibraryFunction)(wchar_t* fullpath, int length, JSON* Data);
	_getExampleLibraryFunction ExampleLibraryFunction;
};
