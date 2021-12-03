// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "LocalEmergenceServer.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Windows/WindowsSystemIncludes.h"

void ULocalEmergenceServer::LaunchLocalServerProcess()
{
	FString EmergenceServerBinariesPath = FString(FWindowsPlatformProcess::BaseDir()) + "/EmergenceEVMLocalServer.exe";
	FString EmergenceServerPluginPath = FString(FPaths::ProjectPluginsDir() + "Emergence/EmergenceServer/EmergenceEVMLocalServer.exe");
	FString LoadPath;
	if (FPaths::FileExists(EmergenceServerBinariesPath)) {
		LoadPath = EmergenceServerBinariesPath;
	}
	else if (FPaths::FileExists(EmergenceServerPluginPath)) {
		LoadPath = EmergenceServerPluginPath;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Couldn't find EmergenceServer in binaries or plugin path locations. Make sure you have the server files copied to Plugins/Emergence/EmergenceServer/EmergenceEVMLocalServer.exe"));
		return;
	}

	if (GConfig) {
		FString EmergenceCustomServerLocation;
		if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("CustomEmergenceServerLocation"), EmergenceCustomServerLocation, GEditorPerProjectIni)) {
			FParse::Value(*EmergenceCustomServerLocation, TEXT("FilePath="), EmergenceCustomServerLocation);
			if (FPaths::FileExists(*EmergenceCustomServerLocation)) {
				LoadPath = EmergenceCustomServerLocation;
				UE_LOG(LogTemp, Warning, TEXT("Found EmergenceServer at override path (%s)."), *EmergenceCustomServerLocation);
			}
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Loading Emergence Server from path: %s"), *LoadPath);

	const FString JsonArgs("\"{\\\"Name\\\":\\\"Crucibletest\\\",\\\"Description\\\":\\\"UnrealEngineWalletConnect\\\",\\\"Icons\\\":\\\"https:\\/\\/crucible.network\\/wp-content\\/uploads\\/2020\\/10\\/cropped-crucible_favicon-32x32.png\\\",\\\"URL\\\":\\\"https:\\/\\/crucible.network\\\"}\"");

	//Add the args
	TArray<FString> Args = {
		"--walletconnect=" + JsonArgs,
		"--processid=" + FString::FromInt(FWindowsPlatformProcess::GetCurrentProcessId())
	};

	//combine the args
	FString ArgString = "";
	for (int i = 0; i < Args.Num(); i++) {
		if (i != 0) { //add a space before the next arg
			ArgString = ArgString + " ";
		}
		UE_LOG(LogTemp, Display, TEXT("calling argument [%d]: %s"), i, *Args[i]);
		ArgString = ArgString + Args[i];
	}
	UE_LOG(LogTemp, Display, TEXT("Total argument lenth is %d"), ArgString.Len());
	//create the process
	FPlatformProcess::CreateProc(*LoadPath, *ArgString, false, false, false, nullptr, 0, nullptr, nullptr);
	UE_LOG(LogTemp, Display, TEXT("calling: %s %s"), *LoadPath, *ArgString);
}

void ULocalEmergenceServer::KillLocalServerProcess()
{
	UHttpHelperLibrary::ExecuteHttpRequest<ULocalEmergenceServer>(nullptr, nullptr, UHttpHelperLibrary::APIBase + "finish");
	UE_LOG(LogTemp, Display, TEXT("KillLocalServerProcess request started..."));
}