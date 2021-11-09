// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EmergenceScreen.h"

UEmergenceUI* UEmergenceScreen::GetEmergenceUI() {
	return Cast<UEmergenceUI>(GetParent()->GetOuter()->GetOuter());
}