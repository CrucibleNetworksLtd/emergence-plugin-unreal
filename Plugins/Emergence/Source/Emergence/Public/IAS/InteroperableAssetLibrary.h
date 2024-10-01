// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "IAS/InteroperableAsset.h"
#include "InteroperableAssetLibrary.generated.h"

UCLASS()
class EMERGENCE_API UEmergenceInteroperableAssetLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, meta=(DeterminesOutputType="ElementClass"))
    static UEmergenceInteroperableAssetElement* GetInteroperableAssetElement(TArray<UEmergenceInteroperableAssetElement*> Array, UPARAM(meta = (AllowAbstract = false))TSubclassOf<UEmergenceInteroperableAssetElement> ElementClass, bool& success);
};