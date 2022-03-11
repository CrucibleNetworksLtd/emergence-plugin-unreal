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
	FFilePath CustomEmergenceServerLocation;
	UPROPERTY(config, EditAnywhere, Category = "Emergence Server")
	FString NodeURL;
	UPROPERTY(config, EditAnywhere, Category = "Emergence Server", meta=(DisplayName = "Custom Game ID"))
	FString GameID;
};