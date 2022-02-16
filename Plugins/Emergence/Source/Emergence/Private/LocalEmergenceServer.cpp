// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "LocalEmergenceServer.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Windows/WindowsSystemIncludes.h"

//Stuff for the windows api stuff, probably should be incapsulated so it doesn't build when we start working on none-windows stuff
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

void ULocalEmergenceServer::LaunchLocalServerProcess(bool LaunchHidden)
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
		UE_LOG(LogEmergenceHttp, Error, TEXT("Couldn't find EmergenceServer in binaries or plugin path locations. Make sure you have the server files copied to Plugins/Emergence/EmergenceServer/EmergenceEVMLocalServer.exe"));
		return;
	}

	if (GConfig) {
		FString EmergenceCustomServerLocation;
		if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("CustomEmergenceServerLocation"), EmergenceCustomServerLocation, GGameIni)) {
			FParse::Value(*EmergenceCustomServerLocation, TEXT("FilePath="), EmergenceCustomServerLocation);
			if (FPaths::FileExists(*EmergenceCustomServerLocation)) {
				LoadPath = EmergenceCustomServerLocation;
				UE_LOG(LogEmergenceHttp, Warning, TEXT("Found EmergenceServer at override path (%s)."), *EmergenceCustomServerLocation);
			}
		}
	}

	UE_LOG(LogEmergenceHttp, Display, TEXT("Loading Emergence Server from path: %s"), *LoadPath);

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
		UE_LOG(LogEmergenceHttp, Display, TEXT("calling argument [%d]: %s"), i, *Args[i]);
		ArgString = ArgString + Args[i];
	}
	UE_LOG(LogEmergenceHttp, Display, TEXT("Total argument lenth is %d"), ArgString.Len());
	//create the process
	FPlatformProcess::CreateProc(*LoadPath, *ArgString, false, true, LaunchHidden, nullptr, 0, nullptr, nullptr);
	UE_LOG(LogEmergenceHttp, Display, TEXT("calling: %s %s"), *LoadPath, *ArgString);
}

void ULocalEmergenceServer::KillLocalServerProcess()
{
	UHttpHelperLibrary::ExecuteHttpRequest<ULocalEmergenceServer>(nullptr, nullptr, UHttpHelperLibrary::APIBase + "finish");
	UE_LOG(LogEmergenceHttp, Display, TEXT("KillLocalServerProcess request started..."));
}

bool ULocalEmergenceServer::GetUsedTCPPorts(TArray<int>& UsedPorts) {
	PMIB_TCPTABLE2 pTcpTable;
	ULONG ulSize = 0;
	DWORD dwRetVal = 0;

	int i;

	pTcpTable = (MIB_TCPTABLE2*)MALLOC(sizeof(MIB_TCPTABLE2));
	if (pTcpTable == NULL) {
		//Error allocating memory
		return false;
	}

	ulSize = sizeof(MIB_TCPTABLE);
	// Make an initial call to GetTcpTable2 to get the necessary size into the ulSize variable
	if ((dwRetVal = GetTcpTable2(pTcpTable, &ulSize, TRUE)) ==
		ERROR_INSUFFICIENT_BUFFER) {
		FREE(pTcpTable);
		pTcpTable = (MIB_TCPTABLE2*)MALLOC(ulSize);
		if (pTcpTable == NULL) {
			//Error allocating memory
			return false;
		}
	}
	// Make a second call to GetTcpTable2 to get the actual data we require
	if ((dwRetVal = GetTcpTable2(pTcpTable, &ulSize, TRUE)) == NO_ERROR) {
		for (i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
			UsedPorts.AddUnique(ntohs((u_short)pTcpTable->table[i].dwLocalPort));
		}
	}
	else {
		//failed
		FREE(pTcpTable);
		return false;
	}

	if (pTcpTable != NULL) {
		FREE(pTcpTable);
		pTcpTable = NULL;
	}

	return true;
}