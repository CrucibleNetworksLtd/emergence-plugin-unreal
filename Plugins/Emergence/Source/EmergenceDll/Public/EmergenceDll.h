// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "EmergenceDeployment.h"

class FEmergenceDllModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool SendTransactionViaKeystore(UEmergenceDeployment* Deployment, FString MethodName, FString PrivateKey, FString PublicKey, FString GasPrice, FString Value, FString& TransactionResponse);

private:
	/** Handle to the test dll we will load */
	void*	ExampleLibraryHandle;
};
