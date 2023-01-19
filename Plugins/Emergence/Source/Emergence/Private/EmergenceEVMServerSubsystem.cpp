// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "EmergenceEVMServerSubsystem.h"
#include "LocalEmergenceServer.h"
#include "HttpService/HttpHelperLibrary.h"

void UEmergenceEVMServerSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
#if UNREAL_MARKETPLACE_BUILD
	UHttpHelperLibrary::APIBase = "https://67.205.183.112/api/";
#else
	bool LaunchHidden = true;
	if (GConfig) {
		GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("LaunchServerHidden"), LaunchHidden, GGameIni);
	}
	ULocalEmergenceServer::LaunchLocalServerProcess(LaunchHidden);
#endif
}

void UEmergenceEVMServerSubsystem::Deinitialize() {
#if !UNREAL_MARKETPLACE_BUILD
	ULocalEmergenceServer::KillLocalServerProcess();
#endif
}