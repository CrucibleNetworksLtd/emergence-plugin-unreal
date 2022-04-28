// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once
#include "Engine/EngineTypes.h"
#include "Chain.h"
#include "EmergencePluginSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class UEmergencePluginSettings : public UObject
{
	GENERATED_BODY()

public:
	UEmergencePluginSettings(const FObjectInitializer& ObjectInitializer);

	//Used to set the location of a custom version of the EVM server. Leave blank for the default.
	UPROPERTY(AdvancedDisplay, config, EditAnywhere, Category = "General", meta = (DisplayName = "[INTERNAL] Custom Emergence Server Location", FilePathFilter = "Emergence Server (EmergenceEVMLocalServer.exe)|EmergenceEVMLocalServer.exe"))
	FFilePath CustomEmergenceServerLocation;

	//An optional Emergence custom Game ID, which allows you to upload new 2d avatars which can be selected from the persona creation screen in the overlay. Leave blank for the default.
	UPROPERTY(config, EditAnywhere, Category = "General", meta = (DisplayName = "Custom Game ID"))
	FString GameID;
	
	UPROPERTY(config, EditAnywhere, Category = "Chain", meta=(ShowOnlyInnerProperties))
	FEmergenceChainStruct Chain;
	
};