// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Element.generated.h"

UCLASS(Abstract, HideDropdown, Category = "JSON|EmergenceInteroperableAsset")
class EMERGENCE_API UEmergenceInteroperableAssetElement : public UObject
{
    GENERATED_BODY()
public:
    FString ElementName;
};