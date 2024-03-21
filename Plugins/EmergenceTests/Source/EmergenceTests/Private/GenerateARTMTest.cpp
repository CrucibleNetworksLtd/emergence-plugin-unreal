// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerateARTMTest.h"
#include "Futurepass/ARTMBuilderLibrary.h"

void AGenerateARTMTest::StartTest()
{
	TArray<FFutureverseARTMOperation> ARTMOperations = {
	FFutureverseARTMOperation(EFutureverseARTMOperationType::DELETELINK, "equippedWith_Exhausts", "did:fv-asset:7672:root:358500:1", "did:fv-asset:7672:root:358500:2"),
	FFutureverseARTMOperation(EFutureverseARTMOperationType::CREATELINK, "equippedWith_Body", "did:fv-asset:7672:root:358501:3", "did:fv-asset:7672:root:358501:4")
	};
	FString GeneratedARTM = UARTMBuilderLibrary::GenerateARTM("Just a test message", ARTMOperations, "0x238678df4F2CeeCC8b09C2b49eb94458682e6A4C", "70");
	FString CheckARTM = L"Asset Registry transaction\n\nJust a test message\n\nOperations:\n\nasset-link delete\n- equippedWith_Exhausts\n- did:fv-asset:7672:root:358500:1\n- did:fv-asset:7672:root:358500:2\nend\n\nasset-link create\n- equippedWith_Body\n- did:fv-asset:7672:root:358501:3\n- did:fv-asset:7672:root:358501:4\nend\n\nOperations END\n\nAddress: 0x238678df4F2CeeCC8b09C2b49eb94458682e6A4C\nNonce: 70";

	if (GeneratedARTM == CheckARTM) {
		FinishTest(EFunctionalTestResult::Succeeded, "");
	}
	else {
		FinishTest(EFunctionalTestResult::Failed, "ARTM didn't match correct message");
	}
}
