// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceLocalWalletConnect.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "EmergenceCore.h"
#include "EmergenceLocalEVMGetURIThread.h"

#define LOCTEXT_NAMESPACE "FEmergenceModule"

FEmergenceLocalWalletConnectModule::_GetURIHandle FEmergenceLocalWalletConnectModule::GetURI = nullptr;

FEmergenceLocalWalletConnectModule::_getRequestToSignHandle FEmergenceLocalWalletConnectModule::RequestToSignHandle = nullptr;

void FEmergenceLocalWalletConnectModule::LoadLibrary()
{
#if PLATFORM_WINDOWS
	if (GetURI) { //if we already have a handle to the function
		return;
	}

	FString BaseDir = IPluginManager::Get().FindPlugin("Emergence")->GetBaseDir();
	FString DllLocation = BaseDir + "/LocalEVM/CloudEVM3.dll";
	FPlatformProcess::AddDllDirectory(*DllLocation);
	FString LibraryPath = FPaths::ConvertRelativePathToFull(DllLocation);

	if (LibraryPath.IsEmpty()) {
		UE_LOG(LogEmergence, Error, TEXT("Failed to load LocalEVMLibrary, library path empty."));
		return; //error out
	}

	LocalEVMLibraryHandle = FPlatformProcess::GetDllHandle(*LibraryPath);

	if (LocalEVMLibraryHandle && !GetURI) { //if we now have a handle (not a guarantee), and we don't have a library
		GetURI = (_GetURIHandle)FPlatformProcess::GetDllExport(LocalEVMLibraryHandle, TEXT("GetQRCode"));
	}

	if (LocalEVMLibraryHandle && !RequestToSignHandle) {
		RequestToSignHandle = (_getRequestToSignHandle)FPlatformProcess::GetDllExport(LocalEVMLibraryHandle, TEXT("RequestToSign"));
	}

	if (!GetURI || !RequestToSignHandle) { //if getting the handle failed, give up now
		UE_LOG(LogEmergence, Error, TEXT("Failed to load LocalEVMLibrary."));
		return; //give up
	}
#endif
}

void FEmergenceLocalWalletConnectModule::FreeLibrary()
{
	FPlatformProcess::FreeDllHandle(LocalEVMLibraryHandle);
	RequestToSignHandle = nullptr;
	GetURI = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceLocalWalletConnectModule, EmergenceLocalWalletConnectModule)