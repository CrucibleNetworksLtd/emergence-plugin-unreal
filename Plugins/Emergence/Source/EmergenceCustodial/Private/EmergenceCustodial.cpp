// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceCustodial.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FEmergenceCustodialModule"

void FEmergenceCustodialModule::StartupModule()
{

}

void FEmergenceCustodialModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceCustodialModule, EmergenceCustodial)