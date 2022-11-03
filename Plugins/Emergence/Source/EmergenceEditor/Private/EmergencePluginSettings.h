// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once
#include "Engine/EngineTypes.h"
#include "Environment.h"
#include "EmergenceChainObject.h"
#include "EmergencePluginSettings.generated.h"

UCLASS(config = Game, defaultconfig)
class UEmergencePluginSettings : public UObject
{
	GENERATED_BODY()

public:
	UEmergencePluginSettings(const FObjectInitializer& ObjectInitializer);

	//Which cloud environment should be communicated with when the game is built as "Debug", "Development" or "Test".
	UPROPERTY(config, EditAnywhere, Category = "General", meta = (EditCondition = "!EnableDevelopmentEnvironment", DisplayName = "Debug/Development/Test build cloud environment"))
	EEmergenceEnvironment DevelopmentEnvironment = EEmergenceEnvironment::Staging;
	
	//Which cloud environment should be communicated with when the game is built as "Shipping".
	UPROPERTY(config, EditAnywhere, Category = "General", meta = (EditCondition = "!EnableDevelopmentEnvironment", DisplayName = "Shipping build cloud environment"))
	EEmergenceEnvironment ShippingEnvironment = EEmergenceEnvironment::Production;
	
	//For Crucible use only! Enable the development environment. Not to be confused with the development build of the game.
	UPROPERTY(AdvancedDisplay, config, EditAnywhere, Category = "General", meta = (DisplayName = "[INTERNAL] Enable Development Environment"))
	bool EnableDevelopmentEnvironment = false;

	//For Crucible use only! Used to set the location of a custom version of the EVM server. Leave blank for the default.
	UPROPERTY(AdvancedDisplay, config, EditAnywhere, Category = "General", meta = (DisplayName = "[INTERNAL] Custom Emergence Server Location", FilePathFilter = "Emergence Server (EmergenceEVMLocalServer.exe)|EmergenceEVMLocalServer.exe"))
	FFilePath CustomEmergenceServerLocation;
	
	//The avatar icon to display if a persona doesn't have an avatar associated to it.
	UPROPERTY(config, EditAnywhere, Category = "General")
	TSoftObjectPtr<UTexture2D> DefaultAvatarIcon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath("Texture2D'/Emergence/Components/UE.UE'"));
	
	//Should the overlay UI show the user's balance?
	UPROPERTY(config, EditAnywhere, Category = "UI Overlay")
	bool ShowBalance = true;

	//The blockchain used in the overlay UI to show the user's balance.
	UPROPERTY(config, NoClear, EditAnywhere, Category = "UI Overlay", meta = (EditCondition = "ShowBalance", DisplayName = "Blockchain used in UI for balance"))
	TSoftObjectPtr<UEmergenceChain> Chain = Cast<UEmergenceChain>(StaticLoadObject(UEmergenceChain::StaticClass(), this, TEXT("/Emergence/Chains/Polygon.Polygon")));

	//The IPFS node to use when getting IPFS data via HTTP. Leaving it blank will use the default "http://ipfs.openmeta.xyz/ipfs/". The IPFS hash will be added to the end (for example, using the default: "http://ipfs.openmeta.xyz/ipfs/Qme7ss3ARVgxv6rXqVPiikMJ8u2NLgmgszg13pYrDKEoiu")
	UPROPERTY(config, EditAnywhere, Category = "IPFS", meta = (DisplayName = "Custom IPFS Node"))
	FString IPFSNode;
	
	//For Crucible use only! Shows the EVM server output window.
	UPROPERTY(AdvancedDisplay, config, EditAnywhere, Category = "General", meta = (DisplayName = "[INTERNAL] Launch EVM server hidden"))
	bool LaunchServerHidden = true;
	
};