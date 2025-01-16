// Copyright Crucible Networks Ltd 2025. All Rights Reserved.

#include "Eliza.h"
DEFINE_LOG_CATEGORY(LogEliza)
DEFINE_LOG_CATEGORY(LogElizaHttp)
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FElizaModule"

void FElizaModule::StartupModule()
{

}

void FElizaModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FElizaModule, Eliza)