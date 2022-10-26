// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/WaitForConfirmations.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UWaitForConfirmations* UWaitForConfirmations::WaitForConfirmations(UObject* WorldContextObject, FString TransactionHash, UEmergenceChain* BlockchainOverride, int NumberOfConfirmations)
{
	UWaitForConfirmations* BlueprintNode = NewObject<UWaitForConfirmations>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->TransactionHash = TransactionHash;
	BlueprintNode->BlockchainOverride = BlockchainOverride;
	BlueprintNode->NumberOfConfirmations = NumberOfConfirmations;
	return BlueprintNode;
}

void UWaitForConfirmations::ActivateByTimer()
{
	UGetTransactionStatus* TransactionStatusCall = UGetTransactionStatus::GetTransactionStatus(this->WorldContextObject, this->TransactionHash, this->BlockchainOverride);
	TransactionStatusCall->OnGetTransactionStatusCompleted.AddDynamic(this, &UWaitForConfirmations::TransactionStatusReturns);
	TransactionStatusCall->Activate();
}

void UWaitForConfirmations::TransactionStatusReturns(FEmergenceTransaction Transaction, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceClientJsonParseFailed) {
		//probably hasn't been picked up by the node yet
		WaitThenGetStatus(); //wait for some seconds and then try to get the transaction status again
	}
	else if (StatusCode == EErrorCode::EmergenceOk) {
		//probably good to read
		if (Transaction.Confirmations > this->NumberOfConfirmations) { //if we have a good amount of confirmations
			//send this transaction
			this->OnWaitForConfirmationsCompleted.Broadcast(Transaction, StatusCode);
		}
		else {
			WaitThenGetStatus(); //wait for some seconds and then try to get the transaction status again
		}
	}
	else {
		//any other error
		//send the error and a blank struct
		this->OnWaitForConfirmationsCompleted.Broadcast(FEmergenceTransaction(), StatusCode);
	}
}

void UWaitForConfirmations::WaitThenGetStatus()
{
	this->WorldContextObject->GetWorld()->GetTimerManager().SetTimer(this->TimerHandle, this, &UWaitForConfirmations::ActivateByTimer, 5.0F, false);
}

void UWaitForConfirmations::Activate()
{
	ActivateByTimer();
}
