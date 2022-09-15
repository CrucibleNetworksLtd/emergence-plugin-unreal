// Fill out your copyright notice in the Description page of Project Settings.


#include "EmergenceChainObject.h"

UEmergenceChain::UEmergenceChain(FString _NodeURL)
{
	this->NodeURL = _NodeURL;
}

UEmergenceChain* UEmergenceChain::GetEmergenceChainDataFromConfig(UObject* Outer)
{
	FString FEmergenceChainStructText;
	if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("Chain"), FEmergenceChainStructText, GGameIni))
	{
		auto FoundObject = Cast<UEmergenceChain>(StaticLoadObject(UEmergenceChain::StaticClass(), Outer, *FEmergenceChainStructText));
		if (FoundObject) {
			return FoundObject;
		}
	}
    return Cast<UEmergenceChain>(StaticLoadObject(UEmergenceChain::StaticClass(), Outer, TEXT("/Emergence/Chains/Polygon.Polygon")));
}

UEmergenceChain* UEmergenceChain::CreateEmergenceChain(FText _Name, FString _NodeURL, FString _Symbol)
{
	UEmergenceChain* EmergenceChain = NewObject<UEmergenceChain>(UEmergenceChain::StaticClass());
	EmergenceChain->Name = _Name;
	EmergenceChain->NodeURL = _NodeURL;
	EmergenceChain->Symbol = _Symbol;
	return EmergenceChain;
}
