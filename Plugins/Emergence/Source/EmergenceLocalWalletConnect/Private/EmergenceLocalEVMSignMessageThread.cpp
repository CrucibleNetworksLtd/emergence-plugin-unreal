// Copyright Crucible Networks Ltd 2025. All Rights Reserved.

#include "EmergenceLocalEVMSignMessageThread.h"
#include "Types/EmergenceErrorCode.h"

bool FLocalEVMSignMessageThreadRunnable::Init()
{
	GetRequestToSignHandle = FModuleManager::Get().GetModuleChecked<FEmergenceLocalWalletConnectModule>("EmergenceLocalWalletConnect").RequestToSignHandle;
	return true;
}

uint32 FLocalEVMSignMessageThreadRunnable::Run()
{
	try {
		char* ReturnedMessage = GetRequestToSignHandle(MessageToSignRef);

		if (!FString(ReturnedMessage).IsEmpty()) {
			UE_LOG(LogTemp, Display, TEXT("LocalRequestToSign Finished"));
			RequestToSignCompletedHandle->Broadcast(ReturnedMessage, EErrorCode::Ok);
		}
		else {
			RequestToSignCompletedHandle->Broadcast("", EErrorCode::EmergenceClientFailed);
		}
		return 0;
	}
	catch (std::exception exception) {
		UE_LOG(LogTemp, Error, TEXT("Found exception: %s"), *FString(exception.what()));
		return 1;
	}
}

void FLocalEVMSignMessageThreadRunnable::Exit()
{

}

void FLocalEVMSignMessageThreadRunnable::Stop()
{
}
