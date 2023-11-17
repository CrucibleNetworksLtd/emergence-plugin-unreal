// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmergenceDllTest.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
//#include "EmergenceDllTestLibrary/ExampleLibrary.h"

#define LOCTEXT_NAMESPACE "FEmergenceDllTestModule"

void FEmergenceDllTestModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("EmergenceDllTest")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/EmergenceDllTestLibrary/Win64/nativehost.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/EmergenceDllTestLibrary/Mac/Release/libExampleLibrary.dylib"));
#endif // PLATFORM_WINDOWS
	FString DllDirectory = BaseDir + "/ThirdParty/EmergenceDllTestLibrary/Win64/";
	FPlatformProcess::AddDllDirectory(*DllDirectory);
	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(TEXT("H:/emergence-plugin-unreal/Plugins/EmergenceDllTest/ThirdParty/EmergenceDllTestLibrary/Win64/nativehost.dll")) : nullptr;

	if (ExampleLibraryHandle)
	{
		typedef int(*_getExampleLibraryFunction)(wchar_t* fullpath, int length);
		FString Name = "entry";
		
		_getExampleLibraryFunction ExampleLibraryFunction = (_getExampleLibraryFunction)FPlatformProcess::GetDllExport(ExampleLibraryHandle, *Name);
		// Call the test function in the third party library that opens a message box
		ExampleLibraryFunction((TCHAR*)*DllDirectory, DllDirectory.Len());
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
}

void FEmergenceDllTestModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceDllTestModule, EmergenceDllTest)
