// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceDll.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "EmergenceChainObject.h"
#include "EmergenceContract.h"

#define LOCTEXT_NAMESPACE "FEmergenceDllModule"

void FEmergenceDllModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("Emergence")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("EmergenceDll/Win64/nativehost.dll"));
#endif // PLATFORM_WINDOWS
	FString DllDirectory = BaseDir + "/EmergenceDll/Win64/";
	FPlatformProcess::AddDllDirectory(*DllDirectory);
	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(TEXT("H:/emergence-plugin-unreal/Plugins/Emergence/EmergenceDll/Win64/nativehost.dll")) : nullptr;

	UEmergenceChain* Chain = NewObject<UEmergenceChain>(UEmergenceChain::StaticClass());
	Chain->ChainID = 5;
	Chain->Name = FText::FromString("Goerli");
	Chain->NodeURL = "https://goerli.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134";
	Chain->Symbol = "ETH";
	UEmergenceContract* Contract = NewObject<UEmergenceContract>(UEmergenceContract::StaticClass());
	Contract->ABI = R"([{"inputs":[{"internalType":"address","name":"countOf","type":"address"}],"name":"GetCurrentCount","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"IncrementCount","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"","type":"address"}],"name":"currentCount","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"}])";
	UEmergenceDeployment* Deployment = NewObject<UEmergenceDeployment>(UEmergenceDeployment::StaticClass());
	Deployment->Address = "0xC9571AaF9EbCa8C08EC37D962310d0Ab9F8A5Dd2";
	Deployment->Blockchain = Chain;
	Deployment->Contract = Contract;
	FString Response;
	bool success = this->SendTransactionViaKeystore(Deployment, "IncrementCount", "197233bb20ca5efda264661b53546828a6de6b531f317338fe3de7b179958d54", "0x33C5fDbf5944971F3db7Cfc1E6268561E50511bd", "", "0", Response);
	if (success) {
		UE_LOG(LogTemp, Display, TEXT("Got transaction JSON %s"), *Response);
	}
	else {
		UE_LOG(LogTemp, Display, TEXT("transaction failed"));
	}
}

void FEmergenceDllModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
}

bool FEmergenceDllModule::SendTransactionViaKeystore(UEmergenceDeployment* Deployment, FString MethodName, FString PrivateKey, FString PublicKey, FString GasPrice, FString Value, FString& TransactionResponse)
{
	FString GasPriceInternal;
	GasPriceInternal.Empty(0);
	if (!GasPrice.IsEmpty()) {
		GasPriceInternal = GasPrice;
	}

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

	char* ABI = new char[Deployment->Contract->ABI.GetCharArray().Num()];
	TCHAR* CharArray = Deployment->Contract->ABI.GetCharArray().GetData();
	for (int i = 0; i < Deployment->Contract->ABI.GetCharArray().Num(); i++) {
		TCHAR c = CharArray[i];
		ABI[i] = (ANSICHAR)c;
	}

	JSON jsonArgs{
		9,
		"Hello World",
		TCHAR_TO_ANSI(Deployment->Address.GetCharArray().GetData()),
		ABI,
		TCHAR_TO_ANSI(Deployment->Blockchain->Name.ToString().GetCharArray().GetData()),
		TCHAR_TO_ANSI(Deployment->Blockchain->NodeURL.GetCharArray().GetData()),
		TCHAR_TO_ANSI(MethodName.GetCharArray().GetData()),
		"local",
		"12345",
		R"(H:\emergence-plugin-unreal\keystore.test)",
		TCHAR_TO_ANSI(FString::FromInt(Deployment->Blockchain->ChainID).GetCharArray().GetData()),
		TCHAR_TO_ANSI(GasPriceInternal.GetCharArray().GetData()),
		TCHAR_TO_ANSI(Value.GetCharArray().GetData()),
		TCHAR_TO_ANSI(PrivateKey.GetCharArray().GetData()),
		TCHAR_TO_ANSI(PublicKey.GetCharArray().GetData()),
		nullptr,
		0
	};
	FString BaseDir = IPluginManager::Get().FindPlugin("Emergence")->GetBaseDir();
	FString DllDirectory = BaseDir + "/EmergenceDll/Win64/";
	if (ExampleLibraryHandle)
	{
		typedef int(*_getExampleLibraryFunction)(wchar_t* fullpath, int length, JSON* Data);
		FString Name = "entry";

		_getExampleLibraryFunction ExampleLibraryFunction = (_getExampleLibraryFunction)FPlatformProcess::GetDllExport(ExampleLibraryHandle, *Name);
		// Call the test function in the third party library that opens a message box
		if (ExampleLibraryFunction) {
			ExampleLibraryFunction((TCHAR*)*DllDirectory, DllDirectory.Len(), &jsonArgs);
			UE_LOG(LogTemp, Display, TEXT("Got transaction JSON %s"), jsonArgs.result);
			TransactionResponse = "";
			for (int i = 0; i < jsonArgs.ResultLength*2; i++) {
				TransactionResponse.AppendChar(jsonArgs.result[i]);
			}
			return true;
		}
		return false;
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
		return false;
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceDllModule, EmergenceDll)
