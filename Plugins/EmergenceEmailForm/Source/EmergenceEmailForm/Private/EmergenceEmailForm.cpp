// Copyright Epic Games, Inc. All Rights Reserved.

#include "EmergenceEmailForm.h"
#include "EmergenceEmailFormStyle.h"
#include "EmergenceEmailFormCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "UObject/ConstructorHelpers.h"
#include "Interfaces/IPluginManager.h"
#include "Templates/SharedPointer.h"
#include "Styling/SlateStyleRegistry.h"

static const FName EmergenceEmailFormTabName("EmergenceEmailForm");

#define LOCTEXT_NAMESPACE "FEmergenceEmailFormModule"

void FEmergenceEmailFormModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FEmergenceEmailFormStyle::Initialize();
	FEmergenceEmailFormStyle::ReloadTextures();

	FEmergenceEmailFormCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FEmergenceEmailFormCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FEmergenceEmailFormModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEmergenceEmailFormModule::RegisterMenus));
	
	//FGlobalTabmanager::Get()->RegisterNomadTabSpawner(EmergenceEmailFormTabName, FOnSpawnTab::CreateRaw(this, &FEmergenceEmailFormModule::OnSpawnPluginTab))
	//	.SetDisplayName(LOCTEXT("FEmergenceEmailFormTabTitle", "EmergenceEmailForm"))
	//	.SetMenuType(ETabSpawnerMenuType::Hidden);

}

void FEmergenceEmailFormModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FEmergenceEmailFormStyle::Shutdown();

	FEmergenceEmailFormCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(EmergenceEmailFormTabName);


}

TSharedRef<SWindow> FEmergenceEmailFormModule::OnSpawnPluginTab()
{
	auto SlateStyle = FSlateStyleRegistry::FindSlateStyle("EmergenceEmailFormStyle");
	
	return SNew(SWindow)
		.Title(FText::FromString(TEXT("Welcome to Emergence!")))
		.ClientSize(FVector2D(600, 400))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::UserSized)
		[
			SNew(SBorder).BorderImage(new FSlateColorBrush(FLinearColor::FromSRGBColor(FColor(62, 62, 62, 255))))
			[
			// Put your tab content here!
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage).Image(SlateStyle->GetBrush("EmergenceEmailForm.Logo"))
			]
			+ SVerticalBox::Slot()
			[
				SNew(SSpacer).Size(FVector2D(1, 10))
			]
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock).Text(FText::FromString("Optionally, if you would like to keep up date with all the latest developments on Emergence and also the Open Meta DAO, please enter your email here:")).AutoWrapText(true)
			].Padding(8.0F, 0.0F)
			+ SVerticalBox::Slot()
			[
				SNew(SSpacer).Size(FVector2D(1, 10))
			]
			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock).Text(FText::FromString("Email:"))
				]
				+ SHorizontalBox::Slot().FillWidth(1.0F).Padding(10.0F, 0.0F)
				[
					SNew(SEditableTextBox)
				]
				+ SHorizontalBox::Slot()
				[
					SNew(SButton).Text(FText::FromString("Send"))
				].Padding(8.0F,0.0F)
			]
			+ SVerticalBox::Slot()
				[
					SNew(SSpacer).Size(FVector2D(1, 10))
				]
			+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock).Text(FText::FromString("Don't show this again:"))
				]
				+ SHorizontalBox::Slot()
				[
					SNew(SCheckBox)
				].Padding(8.0F, 0.0F)
			]
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock).Text(FText::FromString("Privacy & Terms"))
			]
			]
		];
}

void FEmergenceEmailFormModule::PluginButtonClicked()
{
	//FGlobalTabmanager::Get()->TryInvokeTab(EmergenceEmailFormTabName);
	FSlateApplication::Get().AddWindowAsNativeChild(FEmergenceEmailFormModule::OnSpawnPluginTab(), FGlobalTabmanager::Get()->GetRootWindow().ToSharedRef());
}


void FEmergenceEmailFormModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FEmergenceEmailFormCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FEmergenceEmailFormCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceEmailFormModule, EmergenceEmailForm)