// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "IAS/Element.h"
#include "AvatarElement.generated.h"

USTRUCT(Category = "JSON|EmergenceInteroperableAssetAvatarElementInner", BlueprintType)
struct FEmergenceInteroperableAssetAvatarElementInner
{

    GENERATED_BODY()

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetAvatarElementInner", EditAnywhere, BlueprintReadWrite)
    FString ElementName;

    FEmergenceInteroperableAssetAvatarElementInner() {};

    FEmergenceInteroperableAssetAvatarElementInner(FString _json_) {
        FEmergenceInteroperableAssetAvatarElementInner _tmpEmergenceInteroperableAssetAvatarElementInner;

        FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInteroperableAssetAvatarElementInner>(
            _json_,
            &_tmpEmergenceInteroperableAssetAvatarElementInner,
            0, 0);

        ElementName = _tmpEmergenceInteroperableAssetAvatarElementInner.ElementName;
    }

};

UCLASS(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
class EMERGENCE_API UEmergenceInteroperableAssetAvatarElement : public UEmergenceInteroperableAssetElement
{
  GENERATED_BODY()

public:
    UEmergenceInteroperableAssetAvatarElement() {
        ElementName = "NFT";
    }

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|Element", EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
    FEmergenceInteroperableAssetAvatarElementInner EmergenceInteroperableAssetAvatarElement;
    
};