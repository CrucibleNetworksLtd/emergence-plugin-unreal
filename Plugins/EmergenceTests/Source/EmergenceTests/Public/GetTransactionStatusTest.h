// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "WalletService/GetTransactionStatus.h"
#include "Types/EmergenceChain.h"
#include "GetTransactionStatusTest.generated.h"


UCLASS()
class EMERGENCETESTS_API AGetTransactionStatusTest : public AFunctionalTest
{
	GENERATED_BODY()
	
	virtual void StartTest() override {
		UEmergenceChain* Polygon = NewObject<UEmergenceChain>();
		Polygon->Name = FText::FromString("Polygon");
		Polygon->NodeURL = "EMERGENCE_POLYGON";
		Polygon->Symbol = "MATIC";
		Polygon->ChainID = 137;
		auto GetTransactionStatus = UGetTransactionStatus::GetTransactionStatus(this, "0x8e53978141768fc9eb4f1d83f62365e87cab17715ba90871fd409557b43a37ef", Polygon);
		GetTransactionStatus->OnGetTransactionStatusCompleted.AddDynamic(this, &AGetTransactionStatusTest::GetTransactionStatusCompleted);
		GetTransactionStatus->Activate();
	};
	
	UFUNCTION()
	void GetTransactionStatusCompleted(FEmergenceTransaction Transaction, EErrorCode StatusCode){
		if(Transaction.To == "0x074534df6174759a5ae3ad81e3bcdfc0f940f6a6" &&
		Transaction.From == "0x238678df4f2ceecc8b09c2b49eb94458682e6a4c" &&
		Transaction.BlockNumber == "31969711") {
			FinishTest(EFunctionalTestResult::Succeeded, Transaction.BlockNumber);
			return;
		}
		FinishTest(EFunctionalTestResult::Failed, "From, To or BlockNumber incorrect");
	}
};
