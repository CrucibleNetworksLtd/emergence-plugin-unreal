// Fill out your copyright notice in the Description page of Project Settings.


#include "EmergenceContract.h"

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