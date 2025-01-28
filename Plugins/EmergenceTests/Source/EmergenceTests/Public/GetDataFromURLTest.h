// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "GetDataFromURL.h"
#include "GetDataFromURLTest.generated.h"


UCLASS()
class EMERGENCETESTS_API AGetDataFromURLTest : public AFunctionalTest
{
	GENERATED_BODY()
	
	virtual void StartTest() override {
		auto GetDataFromURL = UGetDataFromUrl::GetDataFromUrl(this, "https://ipfs.openmeta.xyz/ipfs/QmbBo9HV5kqNENjGJ7tV4bytSQYmGPTVVMSkLySnmtsKF9");
		GetDataFromURL->OnGetDataFromUrlCompleted.AddDynamic(this, &AGetDataFromURLTest::GetDataFromUrlCompleted);
		GetDataFromURL->Activate();
	};
	
	UFUNCTION()
	void GetDataFromUrlCompleted(const TArray<uint8>& Data, FString String, EErrorCode StatusCode, bool Success){
		if (StatusCode != EErrorCode::Ok) {
			FinishTest(EFunctionalTestResult::Failed, "HTTP status was wrong");
			return;
		}
		
		if (Data.Num() != 250) {
			FinishTest(EFunctionalTestResult::Failed, "Data length was wrong");
			return;
		}

		FinishTest(EFunctionalTestResult::Succeeded, "Found the known Avatar IPFS Data");
		return;
	}
};
