// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EmergenceUI.h"

void UEmergenceUI::SwitchCurrentScreen(UUserWidget* NewScreen) {
	if (CurrentScreenSlotBoundWidget) {
		UUserWidget* TempNewScreen = NewScreen; //keep this around in-case doing the next method deletes whats holding it
		CurrentScreenSlotBoundWidget->RemoveChildAt(0); //can only have one child, this is fine
		CurrentScreenSlotBoundWidget->AddChild(TempNewScreen);
	}
}

void UEmergenceUI::SwitchCurrentScreenByClass(TSubclassOf<UUserWidget> NewScreenClass)
{
	SwitchCurrentScreen(CreateWidget<UUserWidget>(this, NewScreenClass));
}
