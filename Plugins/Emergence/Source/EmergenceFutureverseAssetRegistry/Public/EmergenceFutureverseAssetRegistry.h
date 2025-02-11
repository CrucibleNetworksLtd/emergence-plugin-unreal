// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "HttpService/HttpHelperLibrary.h"

class FEmergenceFutureverseAssetRegistryModule : public IModuleInterface
{
public:

	//the graphQL of the futureverse asset registery depending on environment
	inline static FString GetFutureverseAssetRegistryAPIURL() {

		FString Environment = UHttpHelperLibrary::GetFVEnvironment();

		if (Environment == "Production") {
			//Production Env URL
			return "https://ar-api.futureverse.app/graphql";
		}

		if (Environment == "Development") {
			//Development Env URL
			return "https://ar-api.futureverse.dev/graphql";
		}

		//Staging Env URL
		return "https://ar-api.futureverse.cloud/graphql";
	};
};
