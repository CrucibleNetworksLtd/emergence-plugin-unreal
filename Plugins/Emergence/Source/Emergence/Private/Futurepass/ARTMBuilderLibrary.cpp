// Fill out your copyright notice in the Description page of Project Settings.


#include "Futurepass/ARTMBuilderLibrary.h"

FString UARTMBuilderLibrary::GenerateARTM(FString Message, TArray<FEmergenceFutureverseARTMOperation> ARTMOperations, FString Address, FString Nonce)
{
    TMap< EEmergenceFutureverseARTMOperationType, FString> OperationTypeStrings = {
        {EEmergenceFutureverseARTMOperationType::CREATELINK, "asset-link create"},
        {EEmergenceFutureverseARTMOperationType::DELETELINK, "asset-link delete"}
    };

    FString ARTM = "Asset Registry transaction\n\n";
    ARTM += Message + "\n\n";
    ARTM += "Operations:\n\n";
    for (FEmergenceFutureverseARTMOperation Operation : ARTMOperations) {
        if (Operation.OperationType == EEmergenceFutureverseARTMOperationType::NONE) {
            continue;
        }

        ARTM += *OperationTypeStrings.Find(Operation.OperationType) + "\n";
        if (Operation.OperationType == EEmergenceFutureverseARTMOperationType::CREATELINK) {
            ARTM += "- " + Operation.Slot + "\n";
        }
        ARTM += "- " + Operation.LinkA + "\n";
        ARTM += "- " + Operation.LinkB + "\n";
        ARTM += "end\n\n";
    }
    ARTM += "Operations END\n\n";
    ARTM += "Address: " + Address + "\n";
    ARTM += "Nonce: " + Nonce;
    return ARTM;
}
