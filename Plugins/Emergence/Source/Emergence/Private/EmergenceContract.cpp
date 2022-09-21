// Fill out your copyright notice in the Description page of Project Settings.


#include "EmergenceContract.h"
#include "Dom/JsonObject.h"

UEmergenceContract::UEmergenceContract(FString _ABI)
{
	this->ABI = _ABI;
}

UEmergenceContract* UEmergenceContract::CreateEmergenceContract(FString _ABI)
{
	UEmergenceContract* EmergenceContract = NewObject<UEmergenceContract>(UEmergenceContract::StaticClass());
	EmergenceContract->ABI = _ABI;
	return EmergenceContract;
}

void UEmergenceContract::FindMethods()
{
	this->Methods.Empty();
	if (!this->ABI.IsEmpty()) {
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(this->ABI);
		TSharedPtr<FJsonValue> JsonObject;
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject)) {
			auto ABIArray = JsonObject->AsArray();
			for (int i = 0; i < ABIArray.Num(); i++) {
				TSharedPtr<FJsonObject> InterfaceObject = ABIArray[i].Get()->AsObject();
				if (InterfaceObject->GetStringField("type") == "function") {
					this->Methods.Add(FEmergenceContractMethod(InterfaceObject->GetStringField("name")));
				}
			}
		}
	}
}
