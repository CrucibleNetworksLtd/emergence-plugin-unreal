// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "IAS/Element.h"
#include "ThumbnailsElement.generated.h"

USTRUCT(Category = "Emergence|Interoperable Asset System", BlueprintType)
struct FEmergenceInteroperableAssetThumbnailsElementInnerThumbnail
{

    GENERATED_BODY()

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    FString Id;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    FString Url;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    int Width;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    int Height;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    int CropStyle;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    FString FileFormat;

    FEmergenceInteroperableAssetThumbnailsElementInnerThumbnail() {};

    FEmergenceInteroperableAssetThumbnailsElementInnerThumbnail(
        FString _Id,
        FString _Url,
        int _Width,
        int _Height,
        int _CropStyle,
        FString _FileFormat) {
        Id = _Id;
        Url = _Url;
        Width = _Width;
        Height = _Height;
        CropStyle = _CropStyle;
        FileFormat = _FileFormat;
    }

};

USTRUCT(Category = "Emergence|Interoperable Asset System", BlueprintType)
struct FEmergenceInteroperableAssetThumbnailsElementInner
{

    GENERATED_BODY()

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FEmergenceInteroperableAssetThumbnailsElementInnerThumbnail SmallThumbnail;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FEmergenceInteroperableAssetThumbnailsElementInnerThumbnail LargeThumbnail;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    TArray<FEmergenceInteroperableAssetThumbnailsElementInnerThumbnail> OtherThumbnails;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString ElementName;

    FEmergenceInteroperableAssetThumbnailsElementInner() {};

    FEmergenceInteroperableAssetThumbnailsElementInner(FString _json_) {
        FEmergenceInteroperableAssetThumbnailsElementInner _tmpEmergenceInteroperableAssetThumbnailsElementInner;

        FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInteroperableAssetThumbnailsElementInner>(
            _json_,
            &_tmpEmergenceInteroperableAssetThumbnailsElementInner,
            0, 0);

        ElementName = _tmpEmergenceInteroperableAssetThumbnailsElementInner.ElementName;
        SmallThumbnail = _tmpEmergenceInteroperableAssetThumbnailsElementInner.SmallThumbnail;
        LargeThumbnail = _tmpEmergenceInteroperableAssetThumbnailsElementInner.LargeThumbnail;
        OtherThumbnails = _tmpEmergenceInteroperableAssetThumbnailsElementInner.OtherThumbnails;
    }

};

UCLASS(Category= "Emergence|Interoperable Asset System", BlueprintType)
class EMERGENCE_API UEmergenceInteroperableAssetThumbnailsElement : public UEmergenceInteroperableAssetElement
{
  GENERATED_BODY()

public:
    UEmergenceInteroperableAssetThumbnailsElement() {
        ElementName = "thumbnails";
    }

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
    FEmergenceInteroperableAssetThumbnailsElementInner ThumbnailsElementData;
    
};