// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmergenceToolbar.h"
#include "EmergenceToolbarStyle.h"
#include "EmergenceToolbarCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

static const FName EmergenceToolbarTabName("EmergenceToolbar");

#define LOCTEXT_NAMESPACE "FEmergenceToolbarModule"

void FEmergenceToolbarModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FEmergenceToolbarStyle::Initialize();
	FEmergenceToolbarStyle::ReloadTextures();

	FEmergenceToolbarCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FEmergenceToolbarCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FEmergenceToolbarModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEmergenceToolbarModule::RegisterMenus));
}

void FEmergenceToolbarModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FEmergenceToolbarStyle::Shutdown();

	FEmergenceToolbarCommands::Unregister();
}

void FEmergenceToolbarModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FEmergenceToolbarModule::PluginButtonClicked()")),
							FText::FromString(TEXT("EmergenceToolbar.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FEmergenceToolbarModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FEmergenceToolbarCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FEmergenceToolbarCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);

				/*Section.AddEntry(FToolMenuEntry::InitComboButton(
					"EditCinematics",
					FUIAction(), FOnGetContent::CreateStatic(nullptr),
					//FOnGetContent::CreateStatic(&FLevelEditorToolBar::GenerateCinematicsMenuContent, InCommandList, TWeakPtr<SLevelEditor>(InLevelEditor)),
					LOCTEXT("EditCinematics_Label", "Cinematics"),
					LOCTEXT("EditCinematics_Tooltip", "Displays a list of Level Sequence objects to open in their respective editors"),
					FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.EditMatinee")
				));*/
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceToolbarModule, EmergenceToolbar)