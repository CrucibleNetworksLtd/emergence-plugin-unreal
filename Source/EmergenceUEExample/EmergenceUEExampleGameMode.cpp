// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmergenceUEExampleGameMode.h"
#include "EmergenceUEExampleCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEmergenceUEExampleGameMode::AEmergenceUEExampleGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
