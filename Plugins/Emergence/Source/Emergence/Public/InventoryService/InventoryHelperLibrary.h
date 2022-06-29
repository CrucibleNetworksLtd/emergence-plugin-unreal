// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryService/InventoryByOwner.h"
#include "AvatarService/AvatarByOwner.h"
#include "InventoryHelperLibrary.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceCombinedInventoryItem
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FEmergenceInventoryItem InventoryItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FEmergenceAvatarResult AvatarItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool HasMatchingAvatar = false;
};

UCLASS()
class UInventoryHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static TArray<FEmergenceCombinedInventoryItem> OrganiseInventoryItems(TArray<FEmergenceInventoryItem> InventoryItems, TArray< FEmergenceAvatarResult> Avatars);

	//Given an EmergenceInventoryItemsMetaContent array, find the best image's URL. PNGs are best, then JPEGs, then GIFs. If nothing is found, returns an empty string.
	UFUNCTION(BlueprintPure)
	static FString GetBestDisplayImage(TArray<FEmergenceInventoryItemsMetaContent> Contents);

	UFUNCTION(BlueprintPure)
	static FString GetBestModel(TArray<FEmergenceInventoryItemsMetaContent> Contents);

	UFUNCTION(BlueprintPure)
	static TSet<FString> GetDynamicMetadataCategories(FString DynamicMetadata);
};
