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
	UE_LOG(LogEmergence, Warning, TEXT("Size of wchar_t: %d"), sizeof(wchar_t));
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
	FString EntryName = TEXT("entry");
	FPlatformProcess::AddDllDirectory(*DllDirectory);
	LibraryPath = FPaths::ConvertRelativePathToFull(DllDirectory + "nativehost.dll");
#endif	
#if PLATFORM_MAC
	FString DllDirectory = BaseDir + "/EmergenceDll/Mac/";
	FString EntryName = TEXT("_entry");
	FPlatformProcess::AddDllDirectory(*DllDirectory);
	LibraryPath = FPaths::ConvertRelativePathToFull(DllDirectory + "nativehost.dylib");
#endif	
#if PLATFORM_WINDOWS || PLATFORM_MAC
	if (LibraryPath.IsEmpty()) {
		UE_LOG(LogEmergence, Error, TEXT("Failed to load library, library path empty."));
		WriteMethod->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceInternalError);
		return; //error out
	}
	
	if (!ExampleLibraryHandle) { //if we don't have a handle on the lib already
		ExampleLibraryHandle = FPlatformProcess::GetDllHandle(*LibraryPath);
		if (!ExampleLibraryHandle) { //if we still don't have it after specifically asking for it
			UE_LOG(LogEmergence, Error, TEXT("Failed to get handle for library at path \"%s\"."), *LibraryPath);
			WriteMethod->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceInternalError);
			return; //give up
		}
	}

	if (ExampleLibraryHandle && !ExampleLibraryFunction) { //if we now have a handle (not a guarantee), and we don't have a library
		ExampleLibraryFunction = (_getExampleLibraryFunction)FPlatformProcess::GetDllExport(ExampleLibraryHandle, *EntryName);
		if (!ExampleLibraryFunction) { //if we still don't have it after specifically asking for it
			UE_LOG(LogEmergence, Error, TEXT("Failed to get handle for library function \"%s\"."), *EntryName);
			WriteMethod->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceInternalError);
			return; //give up
		}
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
#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdangling-field"
#endif
	EmergenceLocalEVMJSON* jsonArgs = new EmergenceLocalEVMJSON{
		0, //unused
		"", //unused
		TCHAR_TO_ANSI(Deployment->Address.GetCharArray().GetData()),
		ABI,
		TCHAR_TO_ANSI(Deployment->Blockchain->Name.ToString().GetCharArray().GetData()),
		TCHAR_TO_ANSI(Deployment->Blockchain->NodeURL.GetCharArray().GetData()),
		TCHAR_TO_ANSI(MethodName.GetCharArray().GetData()),
		"", //no longer used
		"", //no longer used
		R"()", //no longer used
		TCHAR_TO_ANSI(FString::FromInt(Deployment->Blockchain->ChainID).GetCharArray().GetData()),
		TCHAR_TO_ANSI(GasPriceInternal.GetCharArray().GetData()),
		TCHAR_TO_ANSI(Value.GetCharArray().GetData()),
		TCHAR_TO_ANSI(PrivateKey.GetCharArray().GetData()),
		TCHAR_TO_ANSI(PublicKey.GetCharArray().GetData()),
		nullptr, //return address
		0 //return length
	};
#if __clang__
#pragma clang diagnostic pop
#endif
	if (ExampleLibraryHandle)
	{
		if (ExampleLibraryFunction) {

			FLocalEVMThreadRunnable* Runnable = new FLocalEVMThreadRunnable();
			Runnable->Data = new EmergenceLocalEVMJSON(*jsonArgs);

//not sure if this block of clang stuff is required anymore, test with Angelo
#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdangling-field"
#endif
			wcscpy(Runnable->fullpath, TCHAR_TO_WCHAR(*LibraryPath));
			Runnable->length = LibraryPath.Len();
			Runnable->ExampleLibraryFunction = ExampleLibraryFunction;
			Runnable->WriteMethod = WriteMethod;
			auto Thread = FRunnableThread::Create(Runnable, TEXT("LocalEVMThread"));
			return;
#if __clang__
#pragma clang diagnostic pop
#endif
		}
	}

	//if it hasn't returned by now something has gone really wrong
	UE_LOG(LogEmergence, Error, TEXT("Failed to load library, handles weren't valid."));
	WriteMethod->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceInternalError);
	return;
#else
	UE_LOG(LogEmergence, Error, TEXT("Sending Transactions via a private key is only available on Windows."));
	WriteMethod->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceInternalError);
	return;
#endif	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceModule, Emergence)