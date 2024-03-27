// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadMethodJSONToStringArrayTest.h"
#include "JsonObjectWrapper.h"
#include "WalletService/EmergenceJSONHelpers.h"

void AReadMethodJSONToStringArrayTest::StartTest()
{
	TSharedPtr<FJsonObject> JsonObject;
	FJsonObjectWrapper WrappedJSONObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create("{\"response\":[\"ipfs://QmeSjSinHpPnmXmspMjwiXyN6zS4E9zccariGR3jxcaWtq/\"]}");
	if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
		WrappedJSONObject.JsonObject = JsonObject;
		TArray<FString> OutputArray;
		if (UEmergenceJSONHelpers::ReadMethodJSONToStringArray(WrappedJSONObject, OutputArray)) {
			if (OutputArray.Num() > 0 && OutputArray[0] == "ipfs://QmeSjSinHpPnmXmspMjwiXyN6zS4E9zccariGR3jxcaWtq/") {
				FinishTest(EFunctionalTestResult::Succeeded, "");
			}
			else {
				FinishTest(EFunctionalTestResult::Failed, "Output array didn't equal input test string");
			}
		}
		else {
			FinishTest(EFunctionalTestResult::Failed, "Wasn't able to parse JSON method");
		}
	}
	else {
		FinishTest(EFunctionalTestResult::Error, "Wasn't able to create testing JSON");
	}
}
