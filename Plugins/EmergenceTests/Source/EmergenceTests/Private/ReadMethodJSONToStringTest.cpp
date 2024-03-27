// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadMethodJSONToStringTest.h"
#include "WalletService/EmergenceJSONHelpers.h"
#include "JsonObjectWrapper.h"
#include "JsonObjectConverter.h"

void AReadMethodJSONToStringTest::StartTest()
{
	TSharedPtr<FJsonObject> JsonObject;
	FJsonObjectWrapper WrappedJSONObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create("{\"response\":[\"ipfs://QmeSjSinHpPnmXmspMjwiXyN6zS4E9zccariGR3jxcaWtq/\"]}");
	if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
		WrappedJSONObject.JsonObject = JsonObject;
		FString OutputString;
		if (UEmergenceJSONHelpers::ReadMethodJSONToString(WrappedJSONObject, OutputString)) {
			if (OutputString == "{\"response\":[\"ipfs://QmeSjSinHpPnmXmspMjwiXyN6zS4E9zccariGR3jxcaWtq/\"]}") {
				FinishTest(EFunctionalTestResult::Succeeded, "");
			}
			else {
				FinishTest(EFunctionalTestResult::Failed, "Output string didn't equal input test string");
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
