// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "Emergence.h"
DEFINE_LOG_CATEGORY(LogEmergence)
DEFINE_LOG_CATEGORY(LogEmergenceHttp)
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "EmergenceChainObject.h"
#include "EmergenceContract.h"
#include "EmergenceLocalEVMThread.h"

#define LOCTEXT_NAMESPACE "FEmergenceModule"

void FEmergenceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin

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
	//bool success = this->SendTransactionViaKeystore(Deployment, "IncrementCount", "197233bb20ca5efda264661b53546828a6de6b531f317338fe3de7b179958d54", "0x33C5fDbf5944971F3db7Cfc1E6268561E50511bd", "", "0", Response);
	//if (success) {
	//	UE_LOG(LogTemp, Display, TEXT("Got transaction JSON %s"), *Response);
	//}
	//else {
	//	UE_LOG(LogTemp, Display, TEXT("transaction failed"));
	//}
}

void FEmergenceModule::ShutdownModule()
{

}

void FEmergenceModule::SendTransactionViaKeystore(UWriteMethod* WriteMethod, UEmergenceDeployment* Deployment, FString MethodName, FString PrivateKey, FString PublicKey, FString GasPrice, FString Value, FString& TransactionResponse)
{
	FString BaseDir = IPluginManager::Get().FindPlugin("Emergence")->GetBaseDir();
	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	FString DllDirectory = BaseDir + "/EmergenceDll/Win64/";
	FPlatformProcess::AddDllDirectory(*DllDirectory);
	LibraryPath = FPaths::ConvertRelativePathToFull(DllDirectory + "nativehost.dll");
#endif // PLATFORM_WINDOWS
	if (LibraryPath.IsEmpty()) { //probably on mac
		return; //error out
	}
	
	if (!ExampleLibraryHandle) { //if we don't have a handle on the lib already
		ExampleLibraryHandle = FPlatformProcess::GetDllHandle(*LibraryPath);
	}

	if (ExampleLibraryHandle && !ExampleLibraryFunction) { //if we now have a handle (not a guarantee), and we don't have a library
		ExampleLibraryFunction = (_getExampleLibraryFunction)FPlatformProcess::GetDllExport(ExampleLibraryHandle, TEXT("entry"));
	}
	
	if(!ExampleLibraryHandle || !ExampleLibraryFunction) { //if we don't have either of them by now
		return; //give up
	}
	

	FString GasPriceInternal;
	GasPriceInternal.Empty(0);
	if (!GasPrice.IsEmpty()) {
		GasPriceInternal = GasPrice;
	}

	char* ABI = new char[Deployment->Contract->ABI.GetCharArray().Num()];
	TCHAR* CharArray = Deployment->Contract->ABI.GetCharArray().GetData();
	for (int i = 0; i < Deployment->Contract->ABI.GetCharArray().Num(); i++) {
		TCHAR c = CharArray[i];
		ABI[i] = (ANSICHAR)c;
	}

	EmergenceLocalEVMJSON* jsonArgs = new EmergenceLocalEVMJSON{
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

	if (ExampleLibraryHandle)
	{
		
		//FString Name = "entry";

		// Call the test function in the third party library that opens a message box
		if (ExampleLibraryFunction) {

			FLocalEVMThreadRunnable* Runnable = new FLocalEVMThreadRunnable();
			Runnable->Data = new EmergenceLocalEVMJSON(*jsonArgs);
			//wchar_t* NewFullpath = static_cast<wchar_t*>((TCHAR*)*LibraryPath);
			const wchar_t* MyWideCharString = (*LibraryPath);
			wcscpy(Runnable->fullpath, MyWideCharString);
			Runnable->length = LibraryPath.Len();
			Runnable->ExampleLibraryFunction = ExampleLibraryFunction;
			Runnable->WriteMethod = WriteMethod;
			auto Thread = FRunnableThread::Create(Runnable, TEXT("LocalEVMThread"));
			//Thread->WaitForCompletion();
			//ExampleLibraryFunction((TCHAR*)*DllDirectory, DllDirectory.Len(), &jsonArgs);

			/*UE_LOG(LogTemp, Display, TEXT("Got transaction JSON %s"), jsonArgs.result);
			TransactionResponse = "";
			for (int i = 0; i < jsonArgs.ResultLength * 2; i++) {
				TransactionResponse.AppendChar(jsonArgs.result[i]);
			}
			WriteMethod->SendTransactionViaKeystoreComplete(TransactionResponse);*/
			return;
		}
		return;
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
		return;
	}

	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceModule, Emergence)