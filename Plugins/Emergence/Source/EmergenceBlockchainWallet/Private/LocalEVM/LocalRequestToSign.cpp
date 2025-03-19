// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "LocalEVM/LocalRequestToSign.h"
#include "EmergenceBlockchainWallet.h"
#include "Modules/ModuleManager.h"

ULocalRequestToSign* ULocalRequestToSign::LocalRequestToSign(UObject* WorldContextObject)
{
	ULocalRequestToSign* BlueprintNode = NewObject<ULocalRequestToSign>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void ULocalRequestToSign::Activate()
{
	FString MessageToSign = "My Name Jeff";
	UE_LOG(LogTemp, Display, TEXT("LocalRequestToSign Activate"));
	FEmergenceBlockchainWalletModule Module = FModuleManager::Get().GetModuleChecked<FEmergenceBlockchainWalletModule>("EmergenceBlockchainWallet");
	char* ResponseMessage = Module.RequestToSignHandle(TCHAR_TO_ANSI(*MessageToSign));
	FString Response = FString(ResponseMessage);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Response);
	UE_LOG(LogTemp, Display, TEXT("LocalRequestToSign Finished"));
}