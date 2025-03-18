// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "LocalEVM/GetWalletConnectURI.h"
#include "Types/EmergenceChain.h"
#include "EmergenceLocalEVM3Thread.h"
#include "EmergenceBlockchainWallet.h"

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

	FLocalEVM3ThreadRunnable* Runnable = new FLocalEVM3ThreadRunnable();
	Runnable->GetURI = FEmergenceBlockchainWalletModule::GetURI;
	Runnable->UriBufferRef = uriBuffer;
	Runnable->StatusRef = &status;
	auto Thread = FRunnableThread::Create(Runnable, TEXT("LocalEVM3Thread"));

	//start a repeating timer
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(RepeatingTimerHandle, this, &UGetWalletConnectURI::UpdateStatus, 0.5f, true);
	/*
	while (uriBuffer[0] == NULL) {
		FString URIAsString = uriBuffer;
		UE_LOG(LogTemp, Display, TEXT("URI: %s"), *URIAsString);
	}

	FString URIAsString = FString(256, uriBuffer);
	UE_LOG(LogTemp, Display, TEXT("URI: %s"), *URIAsString);

	while (status == 2) {
		UE_LOG(LogTemp, Display, TEXT("status: %d"), status);
	}
	UE_LOG(LogTemp, Display, TEXT("status: %d"), status);
	*/
}

void UGetWalletConnectURI::UpdateStatus()
{
	if (uriBuffer[0] != NULL) {
		FString URIAsString = FString(256, uriBuffer);
		UE_LOG(LogTemp, Display, TEXT("URI: %s"), *URIAsString);
	}

	if (status != 2) {
		UE_LOG(LogTemp, Display, TEXT("status: %d"), status);
	}

	if (uriBuffer[0] != NULL && status != 2) {
		UE_LOG(LogTemp, Display, TEXT("DONE!!!"));
		WorldContextObject->GetWorld()->GetTimerManager().SetTimer(RepeatingTimerHandle, this, &UGetWalletConnectURI::UpdateStatus, 0.0f, false);
	}
}
