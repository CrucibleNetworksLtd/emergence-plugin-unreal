#pragma once
#include "Engine/EngineTypes.h"
#include "EmergencePluginSettings.generated.h"

UCLASS(config = EditorPerProjectUserSettings, defaultconfig)
class UEmergencePluginSettings : public UObject
{
	GENERATED_BODY()

public:
	UEmergencePluginSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(config, EditAnywhere, Category = "Emergence Server", meta = (FilePathFilter = "Emergence Server (EmergenceEVMLocalServer.exe)|EmergenceEVMLocalServer.exe"))
	FFilePath CustomEmergenceServerLocation;
};