// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "LocalEVM/LocalRequestToSign.h"
#include "EmergenceBlockchainWallet.h"
#include "Modules/ModuleManager.h"
#include "EmergenceLocalEVMSignMessageThread.h"

ULocalRequestToSign* ULocalRequestToSign::LocalRequestToSign(UObject* _WorldContextObject, FString _MessageToSign)
{
	ULocalRequestToSign* BlueprintNode = NewObject<ULocalRequestToSign>();
	BlueprintNode->WorldContextObject = _WorldContextObject;
	BlueprintNode->MessageToSign = _MessageToSign;
	return BlueprintNode;
}

void ULocalRequestToSign::Activate()
{
	UE_LOG(LogTemp, Display, TEXT("LocalRequestToSign Activate"));
	FEmergenceBlockchainWalletModule Module = FModuleManager::Get().GetModuleChecked<FEmergenceBlockchainWalletModule>("EmergenceBlockchainWallet");

	FLocalEVMSignMessageThreadRunnable* Runnable = new FLocalEVMSignMessageThreadRunnable();
	Runnable->MessageToSignRef = TCHAR_TO_ANSI(*this->MessageToSign);
	Runnable->RequestToSignCompletedHandle = &OnLocalRequestToSignCompleted;
	auto Thread = FRunnableThread::Create(Runnable, TEXT("LocalEVMSignMessageThread"));
}