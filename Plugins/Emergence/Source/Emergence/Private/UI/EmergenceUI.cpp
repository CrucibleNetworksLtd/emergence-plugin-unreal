// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "UI/EmergenceUI.h"

void UEmergenceUI::SwitchCurrentScreen(UUserWidget* NewScreen) {
	if (CurrentScreenSlotBoundWidget) {
		UUserWidget* TempNewScreen = NewScreen; //keep this around in-case doing the next method deletes whats holding it
		CurrentScreenSlotBoundWidget->RemoveChildAt(0); //can only have one child, this is fine
		CurrentScreenSlotBoundWidget->AddChild(TempNewScreen);
		this->CurrentScreenSwitched(TempNewScreen);
	}
}

void UEmergenceUI::SwitchCurrentScreenByClass(TSubclassOf<UUserWidget> NewScreenClass)
{
	SwitchCurrentScreen(CreateWidget<UUserWidget>(this, NewScreenClass));
}

void UEmergenceUI::ShowLoadingMessage(FName MessageId, FText Reason)
{
	LoadingMessages.Add(MessageId, Reason);
}

void UEmergenceUI::HideLoadingMessage(FName MessageId)
{
	LoadingMessages.Remove(MessageId);
}

bool UEmergenceUI::GetMostRecentLoadingMessage(FText& Message)
{
	int32 NumberInMap = this->LoadingMessages.Num();
	if (NumberInMap == 0) {
		return false;
	}

	else {
		TArray<FText> Texts;
		this->LoadingMessages.GenerateValueArray(Texts);
		Message = Texts[NumberInMap - 1];
		return true;
	}
}
