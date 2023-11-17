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

	const char* s = "Hello World";
	const char* _address = "0xC9571AaF9EbCa8C08EC37D962310d0Ab9F8A5Dd2";
	const char* _abi = R"([{"inputs":[{"internalType":"address","name":"countOf","type":"address"}],"name":"GetCurrentCount","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"IncrementCount","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"","type":"address"}],"name":"currentCount","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"}])";
	const char* _network = "Ethereum Goerli";
	const char* _nodeURL = "https://goerli.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134";
	const char* _methodname = "IncrementCount";
	const char* _localaccountname = "local";
	const char* _password = "12345";
	const char* _keystorepath = R"(H:\emergence-plugin-unreal\keystore.test)";
	const char* _chainid = "5";
	const char* _gasPrice = "";
	const char* _value = "0";
	const char* _privateKey = "197233bb20ca5efda264661b53546828a6de6b531f317338fe3de7b179958d54";
	const char* _publicKey = "0x33C5fDbf5944971F3db7Cfc1E6268561E50511bd";
	JSON jsonArgs
	{
		9,
		s,
		_address,
		_abi,
		_network,
		_nodeURL,
		_methodname,
		_localaccountname,
		_password,
		_keystorepath,
		_chainid,
		_gasPrice,
		_value,
		_privateKey,
		_publicKey,
		nullptr,
		0
	};

	if (ExampleLibraryHandle)
	{
		typedef int(*_getExampleLibraryFunction)(wchar_t* fullpath, int length, JSON* Data);
		FString Name = "entry";
		
		_getExampleLibraryFunction ExampleLibraryFunction = (_getExampleLibraryFunction)FPlatformProcess::GetDllExport(ExampleLibraryHandle, *Name);
		// Call the test function in the third party library that opens a message box
		ExampleLibraryFunction((TCHAR*)*DllDirectory, DllDirectory.Len(), &jsonArgs);
		UE_LOG(LogTemp, Display, TEXT("Got transaction JSON %s"), jsonArgs.result);
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
