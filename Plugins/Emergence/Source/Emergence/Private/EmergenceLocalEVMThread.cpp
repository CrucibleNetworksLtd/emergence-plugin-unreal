#include "EmergenceLocalEVMThread.h"


bool FLocalEVMThreadRunnable::Init()
{
	return true;
}

uint32 FLocalEVMThreadRunnable::Run()
{
    Data.b = AssignString(Data.b);
    Data._address = AssignString(Data._address);
    Data._ABI = AssignString(Data._ABI);
    Data._network = AssignString(Data._network);
    Data._nodeUrl = AssignString(Data._nodeUrl);
    Data._methodName = AssignString(Data._methodName);
    Data._localAccountName = AssignString(Data._localAccountName);
    Data._password = AssignString(Data._password);
    Data._keystorePath = AssignString(Data._keystorePath);
    Data._chainID = AssignString(Data._chainID);
    Data._gasPrice = AssignString(Data._gasPrice);
    Data._value = AssignString(Data._value);
    Data._privateKey = AssignString(Data._privateKey);
    Data._publicKey = AssignString(Data._publicKey);
	this->ExampleLibraryFunction(fullpath, length, (void*)&Data, sizeof(Data));
	return 0;
}

void FLocalEVMThreadRunnable::Exit()
{
	UE_LOG(LogTemp, Display, TEXT("Got transaction JSON %s"), (char*)(Data.result));
	FString TransactionResponse = "";
	for (int i = 0; i < (int)Data.ResultLength * 2; i++) {
		TransactionResponse.AppendChar(((char*)(Data.result))[i]);
	}
	WriteMethod->SendTransactionViaKeystoreComplete(TransactionResponse);
}

void FLocalEVMThreadRunnable::Stop()
{
}
