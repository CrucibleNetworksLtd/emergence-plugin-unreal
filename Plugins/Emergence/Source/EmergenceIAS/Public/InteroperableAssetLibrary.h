// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "InteroperableAsset.h"
#include "Element.h"
#include "InteroperableAssetLibrary.generated.h"

UCLASS()
class EMERGENCEIAS_API UEmergenceInteroperableAssetLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, meta=(DeterminesOutputType="ElementClass"), Category = "Emergence|Interoperable Asset System")
    static UEmergenceInteroperableAssetElement* GetInteroperableAssetElement(TArray<UEmergenceInteroperableAssetElement*> Array, UPARAM(meta = (AllowAbstract = false))TSubclassOf<UEmergenceInteroperableAssetElement> ElementClass, bool& success);
};