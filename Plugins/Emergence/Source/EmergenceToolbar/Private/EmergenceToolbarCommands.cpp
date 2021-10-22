// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmergenceToolbarCommands.h"

#define LOCTEXT_NAMESPACE "FEmergenceToolbarModule"

void FEmergenceToolbarCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "EmergenceToolbar", "Execute EmergenceToolbar action", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(PluginAction, "EmergenceToolbar", "Execute EmergenceToolbar action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
