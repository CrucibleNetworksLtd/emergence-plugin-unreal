// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/NamedSlot.h"
#include "EmergenceUI.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UEmergenceUI : public UUserWidget
{
	GENERATED_BODY()
public:

	//Switches the active screen to a new widget object
	UFUNCTION(BlueprintCallable)
	void SwitchCurrentScreen(UUserWidget* NewScreen);

	//Switches the active screen to a new widget class
	UFUNCTION(BlueprintCallable)
	void SwitchCurrentScreenByClass(TSubclassOf<UUserWidget> NewScreenClass);

	//The current screen slot sub-component. This is a C++ "BindWidget"'d widget, DO NOT RENAME IN UMG BLUEPRINTS!
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget), Category = "C++ Bind Widgets")
	UNamedSlot* CurrentScreenSlotBoundWidget;
};
