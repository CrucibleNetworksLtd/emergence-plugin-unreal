// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once
#include "Engine/EngineTypes.h"
#include "EmergencePluginSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class UEmergencePluginSettings : public UObject
{
	GENERATED_BODY()

public:
	UEmergencePluginSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(config, EditAnywhere, Category = "Emergence Server", meta = (FilePathFilter = "Emergence Server (EmergenceEVMLocalServer.exe)|EmergenceEVMLocalServer.exe"))
	//Used to set the location of a custom version of the EVM server. Leave blank for the default.
	FFilePath CustomEmergenceServerLocation;

	UPROPERTY(config, EditAnywhere, Category = "Emergence Server")
	FString NodeURL;

	UPROPERTY(config, EditAnywhere, Category = "Emergence Server", meta=(DisplayName = "Custom Game ID"))
	//An optional Emergence custom Game ID, which allows you to upload new 2d avatars which can be selected from the persona creation screen in the overlay. Leave blank for the default.
	FString GameID;

	UPROPERTY(config, EditAnywhere, Category = "Emergence Server")
	//This is the symbol which will be shown as the units for the user's account balance. This should match the token of the blockchain your game is set to connect to. By default this is "MATIC".
	FString TokenSymbol = "MATIC";
};