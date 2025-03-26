#include "EmergenceLocalEVMGetURIThread.h"

bool FLocalEVMGetURIThreadRunnable::Init()
{
	GetURI = FModuleManager::Get().GetModuleChecked<FEmergenceBlockchainWalletModule>("EmergenceBlockchainWallet").GetURI;
	return true;
}

uint32 FLocalEVMGetURIThreadRunnable::Run()
{
	try {
		GetURI(*UriBufferRef, *StatusRef);
	}
	catch (std::exception exception) {
		UE_LOG(LogTemp, Error, TEXT("Found exception: %s"), *FString(exception.what()));
	}
	return 0;
}

void FLocalEVMGetURIThreadRunnable::Exit()
{

}

void FLocalEVMGetURIThreadRunnable::Stop()
{
}
