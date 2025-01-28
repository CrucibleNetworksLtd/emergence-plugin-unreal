// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "Types/EmergenceErrorCode.h"
#include "StatusCodeEnumToStatusCodeIntTest.generated.h"


UCLASS()
class EMERGENCETESTS_API AStatusCodeEnumToStatusCodeIntTest : public AFunctionalTest
{
	GENERATED_BODY()
	
	virtual void StartTest() override {
		if(UEmergenceErrorCode::Conv_ErrorCodeToInt(EErrorCode::EmergenceOk) != 0){
			FinishTest(EFunctionalTestResult::Failed, "Emergence okay was the wrong code ");
			return;
		}
		
		if(UEmergenceErrorCode::Conv_ErrorCodeToInt(EErrorCode::Ok) != 200){
			FinishTest(EFunctionalTestResult::Failed, "Http ok was the wrong code");
			return;
		}
		
		if(UEmergenceErrorCode::Conv_ErrorCodeToInt(EErrorCode::EmergenceClientFailed) != -1){
			FinishTest(EFunctionalTestResult::Failed, "Emergence client failed was the wrong code ");
			return;
		}
		
		FinishTest(EFunctionalTestResult::Succeeded, "All test codes passed");
		return;
	};

};
