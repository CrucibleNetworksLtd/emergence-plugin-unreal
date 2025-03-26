// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "LocalEVM/GetWalletConnectURI.h"
#include "Types/EmergenceChain.h"
#include "EmergenceLocalEVMGetURIThread.h"
#include "EmergenceLocalWalletConnect.h"

UGetWalletConnectURI* UGetWalletConnectURI::GetWalletConnectURI(UObject* WorldContextObject)
{
	UGetWalletConnectURI* BlueprintNode = NewObject<UGetWalletConnectURI>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UGetWalletConnectURI::Activate()
{
	uriBuffer = new UTF16CHAR[256];
	FMemory::Memzero(uriBuffer, sizeof(UTF16CHAR) * 256);
	status = -1;


	FEmergenceLocalWalletConnectModule Module = FModuleManager::Get().GetModuleChecked<FEmergenceLocalWalletConnectModule>("EmergenceLocalWalletConnect");

	//Currently, doing this will reset the internal state of the WalletConnect Library. @TODO do this more cleanly
	Module.FreeLibrary();
	Module.LoadLibrary();

	FLocalEVMGetURIThreadRunnable* Runnable = new FLocalEVMGetURIThreadRunnable();
	Runnable->UriBufferRef = uriBuffer;
	Runnable->StatusRef = &status;
	auto Thread = FRunnableThread::Create(Runnable, TEXT("LocalEVM3Thread"));

	//start a repeating timer
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(RepeatingTimerHandle, this, &UGetWalletConnectURI::UpdateStatus, 0.5f, true);

	//If the user hasn't connected after 5 min, time out the thing before 5 min @TODO figure out why it only stays open for 5 min
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(TimeoutTimerHandle, this, &UGetWalletConnectURI::Timeout, 300.0f, false);
}

void UGetWalletConnectURI::UpdateStatus()
{
	//status codes
	//2: nothing has happened yet
	//1: success, user connected!
	//0: fail, wasn't acknowledged in time

	if (uriBuffer[0] != NULL && !HasReportedWalletConnectURI) { //we have a walletconnect string
		FString URIAsString = FString(256, uriBuffer);
		HasReportedWalletConnectURI = true;
		WalletConnectStateChanged.Broadcast(URIAsString, EWalletConnectState::WaitingForConnection);
		return;
	}

	if (status != 2) { //finished

		//Clear the timers
		WorldContextObject->GetWorld()->GetTimerManager().SetTimer(RepeatingTimerHandle, this, &UGetWalletConnectURI::UpdateStatus, 0.0f, false);
		WorldContextObject->GetWorld()->GetTimerManager().SetTimer(TimeoutTimerHandle, this, &UGetWalletConnectURI::UpdateStatus, 0.0f, false);

		FString URIAsString = FString(256, uriBuffer);

		if (status == 1) { //if status == 0, success, lets go!
			WalletConnectStateChanged.Broadcast(URIAsString, EWalletConnectState::ConnectionSuccessful);
		}
		
		if (status == 0) { //otherwise, report that we've finished but it wasn't a success.
			WalletConnectStateChanged.Broadcast(URIAsString, EWalletConnectState::ConnectionRefused);
		}
	}
}

void UGetWalletConnectURI::Timeout()
{
	//Clear the timer
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(RepeatingTimerHandle, this, &UGetWalletConnectURI::UpdateStatus, 0.0f, false);

	WalletConnectStateChanged.Broadcast(FString(), EWalletConnectState::ConnectionTimedout);
}
