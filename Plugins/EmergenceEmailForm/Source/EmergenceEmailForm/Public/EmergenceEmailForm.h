// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Brushes/SlateImageBrush.h"

class FToolBarBuilder;
class FMenuBuilder;

class FEmergenceEmailFormModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();

	UFUNCTION()
	FReply OnSendButtonClicked();

	UFUNCTION()
	void OnEmailBoxTextChanged(const FText &Text);
private:
	
	void RegisterMenus();

	UPROPERTY()
	TSharedPtr<FSlateImageBrush> Brush;

	TSharedRef<class SWindow> OnSpawnPluginTab();

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
	FString Email;
	TSharedPtr<SEditableTextBox> MyEditableTextBox;
};
