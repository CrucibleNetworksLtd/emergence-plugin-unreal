// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "UI/EmergenceScreen.h"
#include "GenericPlatform/GenericPlatformMisc.h"

UEmergenceUI* UEmergenceScreen::GetEmergenceUI() {
	if (this != nullptr 
		&& GetParent() 
		&& GetParent()->GetOuter() 
		&& GetParent()->GetOuter()->GetOuter()) {
		return Cast<UEmergenceUI>(GetParent()->GetOuter()->GetOuter());
	}
	else {
		return nullptr;
	}
}

void UEmergenceScreen::CopyStringToClipboard(FString StringToCopy)
{
	FPlatformMisc::ClipboardCopy(*StringToCopy);
}
