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
#include "Widgets/Input/SRichTextHyperlink.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "SHyperlinkLaunchURL.h"
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
	FSlateFontInfo WindowFont = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12);
	const FTextBlockStyle LinkStyle = FTextBlockStyle()
		.SetFont(WindowFont)
		.SetColorAndOpacity(FSlateColor(FLinearColor::Blue))
		.SetUnderlineBrush(FSlateColorBrush(FLinearColor::Blue));

	return SNew(SWindow)
		.Title(FText::FromString(TEXT("Welcome to Emergence!")))
		.ClientSize(FVector2D(600, 400))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		[
			SNew(SBorder)
			.BorderImage(new FSlateColorBrush(FLinearColor::FromSRGBColor(FColor(62, 62, 62, 255))))
			[
			// Put your tab content here!
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage).Image(SlateStyle->GetBrush("EmergenceEmailForm.Logo"))
			]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SSpacer).Size(FVector2D(1, 10))
			]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Optionally, if you would like to keep up date with all the latest developments on Emergence and also the Open Meta DAO, please enter your email here:"))
				.AutoWrapText(true)
				.Font(WindowFont)
			].Padding(8.0F, 0.0F)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SSpacer).Size(FVector2D(1, 10))
			]
			+ SVerticalBox::Slot().AutoHeight()
				[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().Padding(0, 2.0f, 0, 0)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Email:"))
					.Font(WindowFont)
				]
				+ SHorizontalBox::Slot().Padding(10.0F, 0.0F)
				[
					SNew(SEditableTextBox).OnTextChanged_Raw(this, &FEmergenceEmailFormModule::OnEmailBoxTextChanged)
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton).Content()[
						SNew(STextBlock).Text(FText::FromString("Send"))
						.Font(WindowFont)
						
					].OnClicked_Raw(this, &FEmergenceEmailFormModule::OnSendButtonClicked)
				].Padding(8.0F,0.0F)
			]
			+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SSpacer).Size(FVector2D(1, 10))
				]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(STextBlock).Text(FText::FromString("Don't show this again:")).Font(WindowFont)
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SCheckBox).IsChecked(true)
				].Padding(8.0F, 0.0F)
			]
			+ SVerticalBox::Slot().FillHeight(1.0f)
				[
					SNew(SSpacer).Size(FVector2D(1, 10))
				]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.Padding(0.0f, 8.0f, 0.0f, 0.0f)
			[
			SNew(SHyperlinkLaunchURL, TEXT("https://www.emergence.site/assets/documents/Emergence_Privacy%20Policy_FINAL%20(25%20Nov%2021).pdf"))
			.Text((FText::FromString("Privacy & Terms")))
			]
		]
		];
}

void FEmergenceEmailFormModule::PluginButtonClicked()
{
	//FGlobalTabmanager::Get()->TryInvokeTab(EmergenceEmailFormTabName);
	FSlateApplication::Get().AddWindowAsNativeChild(FEmergenceEmailFormModule::OnSpawnPluginTab(), FGlobalTabmanager::Get()->GetRootWindow().ToSharedRef());
}

FReply FEmergenceEmailFormModule::OnSendButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Sending an email to: %s"), *this->Email);
	return FReply::Handled();
}

void FEmergenceEmailFormModule::OnEmailBoxTextChanged(const FText& Text)
{
	this->Email = Text.ToString();
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