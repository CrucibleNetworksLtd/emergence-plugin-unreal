// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "IAS/Element.h"
#include "NFTElement.generated.h"


USTRUCT(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", BlueprintType)
struct FEmergenceInteroperableAssetNFTElementInnerAssetMediaType
{

    GENERATED_BODY()

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner|MediaType", EditAnywhere, BlueprintReadWrite)
    FString Type;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner|MediaType", EditAnywhere, BlueprintReadWrite)
    FString Element;

    FEmergenceInteroperableAssetNFTElementInnerAssetMediaType() {};

    FEmergenceInteroperableAssetNFTElementInnerAssetMediaType(FString _Type, FString _Element) {

        Type = _Type;
        Element = _Element;

    }

};

USTRUCT(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", BlueprintType)
struct FEmergenceInteroperableAssetNFTElementInnerAsset
{

    GENERATED_BODY()

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner|Asset", EditAnywhere, BlueprintReadWrite)
    FEmergenceInteroperableAssetNFTElementInnerAssetMediaType MediaType;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner|Asset", EditAnywhere, BlueprintReadWrite)
    FString AssetLocation;

    FEmergenceInteroperableAssetNFTElementInnerAsset() {};

    FEmergenceInteroperableAssetNFTElementInnerAsset(FEmergenceInteroperableAssetNFTElementInnerAssetMediaType _MediaType, FString _AssetLocation) {

        MediaType = _MediaType;
        AssetLocation = _AssetLocation;

    }

};

USTRUCT(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", BlueprintType)
struct FEmergenceInteroperableAssetNFTElementInnerChain
{

    GENERATED_BODY()

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner|Chain", EditAnywhere, BlueprintReadWrite)
    bool IsTestnet;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner|Chain", EditAnywhere, BlueprintReadWrite)
    FString ChainName;

    FEmergenceInteroperableAssetNFTElementInnerChain() {};

    FEmergenceInteroperableAssetNFTElementInnerChain(bool _IsTestnet, FString _ChainName) {

        IsTestnet = _IsTestnet;
        ChainName = _ChainName;

    }

};

USTRUCT(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", BlueprintType)
struct FEmergenceInteroperableAssetNFTElementInner
{

    GENERATED_BODY()

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    FString Address;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    FString NFTName;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    TMap<FString, FString> Attributes;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    FEmergenceInteroperableAssetNFTElementInnerChain Chain;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    FString TokenNumber;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    int32 TokenType;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    FString CollectionName;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    int32 PrimaryAsset;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    FString ElementName;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    FString Creator;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    FString Owner;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAssetNFTElementInner", EditAnywhere, BlueprintReadWrite)
    TArray<FEmergenceInteroperableAssetNFTElementInnerAsset> Assets;

    FEmergenceInteroperableAssetNFTElementInner() {};

    FEmergenceInteroperableAssetNFTElementInner(FString _Address, FString _NFTName, FString _Description, TMap<FString,FString> _Attributes, FEmergenceInteroperableAssetNFTElementInnerChain _Chain, FString _TokenNumber, int32 _TokenType, FString _CollectionName, int32 _PrimaryAsset, FString _ElementName, FString _Creator, FString _Owner, TArray<FEmergenceInteroperableAssetNFTElementInnerAsset> _Assets) {

        Address = _Address;
        NFTName = _NFTName;
        Description = _Description;
        Attributes = _Attributes;
        Chain = _Chain;
        TokenNumber = _TokenNumber;
        TokenType = _TokenType;
        CollectionName = _CollectionName;
        PrimaryAsset = _PrimaryAsset;
        ElementName = _ElementName;
        Creator = _Creator;
        Owner = _Owner;
        Assets = _Assets;

    }

    /* Don't Forget to setup your project
    Add #include "Runtime/JsonUtilities/Public/JsonObjectConverter.h" in
    file with this structs.
    Also you need add "Json", "JsonUtilities" in Build.cs */

    FEmergenceInteroperableAssetNFTElementInner(FString _json_) {
        FEmergenceInteroperableAssetNFTElementInner _tmpEmergenceInteroperableAssetNFTElementInner;

        FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInteroperableAssetNFTElementInner>(
            _json_,
            &_tmpEmergenceInteroperableAssetNFTElementInner,
            0, 0);

        Address = _tmpEmergenceInteroperableAssetNFTElementInner.Address;
        NFTName = _tmpEmergenceInteroperableAssetNFTElementInner.NFTName;
        Description = _tmpEmergenceInteroperableAssetNFTElementInner.Description;
        //Attributes = _tmpEmergenceInteroperableAssetNFTElementInner.Attributes; //@TODO deal with attributes
        Chain = _tmpEmergenceInteroperableAssetNFTElementInner.Chain;
        TokenNumber = _tmpEmergenceInteroperableAssetNFTElementInner.TokenNumber;
        TokenType = _tmpEmergenceInteroperableAssetNFTElementInner.TokenType;
        CollectionName = _tmpEmergenceInteroperableAssetNFTElementInner.CollectionName;
        PrimaryAsset = _tmpEmergenceInteroperableAssetNFTElementInner.PrimaryAsset;
        ElementName = _tmpEmergenceInteroperableAssetNFTElementInner.ElementName;
        Creator = _tmpEmergenceInteroperableAssetNFTElementInner.Creator;
        Owner = _tmpEmergenceInteroperableAssetNFTElementInner.Owner;
        Assets = _tmpEmergenceInteroperableAssetNFTElementInner.Assets;
    }

};

UCLASS(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
class EMERGENCE_API UEmergenceInteroperableAssetNFTElement : public UEmergenceInteroperableAssetElement
{
  GENERATED_BODY()

public:
    UEmergenceInteroperableAssetNFTElement() {
        ElementName = "NFT";
    }

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|Element", EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
    FEmergenceInteroperableAssetNFTElementInner EmergenceInteroperableAssetNFTElement;
    
};