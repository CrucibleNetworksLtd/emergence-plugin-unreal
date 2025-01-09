// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmergenceUEExampleGameMode.h"
#include "EmergenceUEExampleCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EmergenceSingleton.h"

AEmergenceUEExampleGameMode::AEmergenceUEExampleGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AEmergenceUEExampleGameMode::PerformCustodialLogin()
{
	auto Singleton = UEmergenceSingleton::GetEmergenceManager(this);
	Singleton->OnLoginFinished.AddUniqueDynamic(this, &AEmergenceUEExampleGameMode::OnLoginFinalized);
	auto CustodialLogin = UCustodialLogin::CustodialLogin(this);
	CustodialLogin->Activate();
}

void AEmergenceUEExampleGameMode::OnLoginFinalized(EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		auto Singleton = UEmergenceSingleton::GetEmergenceManager(this);
		FString ChecksummedAddress = Singleton->GetCachedAddress(true);
		UE_LOG(LogTemp, Display, TEXT("User is authenticated. Wallet address: %s"), *ChecksummedAddress);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Login failed with error %d"), StatusCode);
	}
}