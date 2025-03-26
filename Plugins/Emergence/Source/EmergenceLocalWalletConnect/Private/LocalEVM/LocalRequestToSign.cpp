// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "LocalEVM/LocalRequestToSign.h"
#include "EmergenceLocalWalletConnect.h"
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
	FEmergenceLocalWalletConnectModule Module = FModuleManager::Get().GetModuleChecked<FEmergenceLocalWalletConnectModule>("EmergenceLocalWalletConnect");

	FLocalEVMSignMessageThreadRunnable* Runnable = new FLocalEVMSignMessageThreadRunnable();
	Runnable->MessageToSignRef = TCHAR_TO_ANSI(*this->MessageToSign);
	Runnable->RequestToSignCompletedHandle = &OnLocalRequestToSignCompleted;
	auto Thread = FRunnableThread::Create(Runnable, TEXT("LocalEVMSignMessageThread"));
}