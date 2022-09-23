// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "Interfaces/ITargetPlatformModule.h"
#include "Modules/ModuleManager.h"
#include "UObject/Package.h"
#include "UObject/WeakObjectPtr.h"
#include "EmergenceContractMethodGraphPanelPinFactory.h"
#define LOCTEXT_NAMESPACE "EmergenceEditorMethodGraphPinModule"

class FEmergenceEditorMethodGraphPinModule : public IModuleInterface
{
	TSharedPtr<FEmergenceContractMethodGraphPanelPinFactory> EmergenceContractMethodGraphPanelPinFactory;

	virtual void StartupModule() override
	{
		EmergenceContractMethodGraphPanelPinFactory = MakeShareable(new FEmergenceContractMethodGraphPanelPinFactory());
		FEdGraphUtilities::RegisterVisualPinFactory(EmergenceContractMethodGraphPanelPinFactory);
	}

	virtual void ShutdownModule() override
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(EmergenceContractMethodGraphPanelPinFactory);
	}

	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEmergenceEditorMethodGraphPinModule, EmergenceEditorMethodGraphPin );