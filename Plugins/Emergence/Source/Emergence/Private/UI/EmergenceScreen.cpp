// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EmergenceScreen.h"

UEmergenceUI* UEmergenceScreen::GetEmergenceUI() {
	if (GetParent() && GetParent()->GetOuter() && GetParent()->GetOuter()->GetOuter()) {
		return Cast<UEmergenceUI>(GetParent()->GetOuter()->GetOuter());
	}
	else {
		return nullptr;
	}
}