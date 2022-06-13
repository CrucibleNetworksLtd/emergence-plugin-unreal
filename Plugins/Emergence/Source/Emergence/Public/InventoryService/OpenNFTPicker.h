// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "InventoryService/EmergenceInventoryServiceStructs.h"
#include "InventoryService/InventoryScreen.h"
#include "OpenNFTPicker.generated.h"

UENUM(BlueprintType)
enum class EEmergenceNFTPickerError : uint8 {
	Ok = 0,
	UserNotConnected = 1,
	UserSwitchedScreen = 2,
	UserClosedOverlay = 3,
	NoPlayerController = 4
};

UCLASS()
class EMERGENCE_API UOpenNFTPicker : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Emergence Requests|Inventory Service")
	static UOpenNFTPicker* OpenNFTPicker(const UObject* WorldContextObject, APlayerController* PlayerController, const FEmergenceInventoryFilterSet& Filters);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSelected, FEmergenceInventoryItem, SelectedItem, EEmergenceNFTPickerError, SelectionError);

	UPROPERTY(BlueprintAssignable)
    FOnItemSelected OnSelectionCompleted;

	UFUNCTION()
	void ItemSelectionCompleted(FEmergenceInventoryItem Item);
	
	UFUNCTION()
	void EmergenceOverlayScreenSwitched(UUserWidget* NewScreen);

	UFUNCTION()
	void EmergenceOverlayClosed();

	UFUNCTION()
	void AfterOverlayCloseCleanup();

	UFUNCTION()
	void EmergenceOverlayReady();

private:
    FEmergenceInventoryFilterSet Filters;
    APlayerController* OpeningPlayerController;
	bool PreviousMouseShowState;
	int PreviousGameInputMode;
	UInventoryScreen* InventoryScreen;
	UEmergenceUI* EmergenceUI;

	const UObject* WorldContextObject;
};
