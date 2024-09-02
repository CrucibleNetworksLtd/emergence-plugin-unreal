// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Emergence.h" //for the log categories
//DECLARE_LOG_CATEGORY_EXTERN(LogEmergenceHttp, Log, All);

class FEmergenceCustodialModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
