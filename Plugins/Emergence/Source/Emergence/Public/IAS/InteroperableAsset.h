// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonValue.h"
#include "InteroperableAsset.generated.h"

USTRUCT(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
struct FEmergenceInteroperableAssetSubTypeChain
{

  GENERATED_BODY()

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|Chain", EditAnywhere, BlueprintReadWrite)
  bool IsTestnet;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|Chain", EditAnywhere, BlueprintReadWrite)
  FString ChainName;

  FEmergenceInteroperableAssetSubTypeChain() {};

  FEmergenceInteroperableAssetSubTypeChain( bool _IsTestnet, FString _ChainName ){

    IsTestnet = _IsTestnet;
    ChainName = _ChainName;
  
  }
  
};

USTRUCT(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
struct FEmergenceInteroperableAssetSubTypeAttribute
{

  GENERATED_BODY()

  FEmergenceInteroperableAssetSubTypeAttribute() {};
  
};

UCLASS(Abstract, Category = "JSON|EmergenceInteroperableAsset")
class EMERGENCE_API UEmergenceInteroperableAssetSubType : public UObject
{
    GENERATED_BODY()
};

USTRUCT(Category = "JSON|EmergenceInteroperableAsset", BlueprintType)
struct FEmergenceInteroperableAssetNFTSubTypeInner
{

    GENERATED_BODY()

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite)
    FString SubTypeName;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite)
    FString NFTName;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite)
    FString Creator;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite)
    FString Owner;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite)
    FString MintDate;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite)
    FEmergenceInteroperableAssetSubTypeAttribute Attributes;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite)
    FEmergenceInteroperableAssetSubTypeChain Chain;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite)
    FString TokenNumber;

    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite)
    int32 TokenType;

    FEmergenceInteroperableAssetNFTSubTypeInner() {};

    FEmergenceInteroperableAssetNFTSubTypeInner(FString _NFTName, FString _Description, FString _Creator, FString _Owner, FString _MintDate, FEmergenceInteroperableAssetSubTypeAttribute _Attributes, FEmergenceInteroperableAssetSubTypeChain _Chain, FString _TokenNumber, int32 _TokenType, FString _SubTypeName) {
        NFTName = _NFTName;
        Description = _Description;
        Creator = _Creator;
        Owner = _Owner;
        MintDate = _MintDate;
        Attributes = _Attributes;
        Chain = _Chain;
        TokenNumber = _TokenNumber;
        TokenType = _TokenType;
        SubTypeName = _SubTypeName;
    }

};

UCLASS(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
class EMERGENCE_API UEmergenceInteroperableAssetNFTSubType : public UEmergenceInteroperableAssetSubType
{
  GENERATED_BODY()

public:
    UPROPERTY(Category = "JSON|EmergenceInteroperableAsset|SubType", EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
    FEmergenceInteroperableAssetNFTSubTypeInner EmergenceInteroperableAssetNFTSubType;
  
};



USTRUCT(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
struct FEmergenceInteroperableAssetAssetAssetTypeMetadatum
{

  GENERATED_BODY()

  FEmergenceInteroperableAssetAssetAssetTypeMetadatum() {};
  
};

USTRUCT(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
struct FEmergenceInteroperableAssetAssetMediaType
{

  GENERATED_BODY()

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|MediaType", EditAnywhere, BlueprintReadWrite)
  FString Type;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|MediaType", EditAnywhere, BlueprintReadWrite)
  FString SubType;

  FEmergenceInteroperableAssetAssetMediaType() {};

  FEmergenceInteroperableAssetAssetMediaType( FString _Type, FString _SubType ){

    Type = _Type;
    SubType = _SubType;
  
  }
  
};

USTRUCT(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
struct FEmergenceInteroperableAssetAsset
{

  GENERATED_BODY()

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|Asset", EditAnywhere, BlueprintReadWrite)
  FEmergenceInteroperableAssetAssetMediaType MediaType;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|Asset", EditAnywhere, BlueprintReadWrite)
  FString AssetLocation;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|Asset", EditAnywhere, BlueprintReadWrite)
  FEmergenceInteroperableAssetAssetAssetTypeMetadatum AssetTypeMetadata;

  FEmergenceInteroperableAssetAsset() {};

  FEmergenceInteroperableAssetAsset( FEmergenceInteroperableAssetAssetMediaType _MediaType, FString _AssetLocation, FEmergenceInteroperableAssetAssetAssetTypeMetadatum _AssetTypeMetadata ){

    MediaType = _MediaType;
    AssetLocation = _AssetLocation;
    AssetTypeMetadata = _AssetTypeMetadata;
  
  }
  
};

USTRUCT(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
struct FEmergenceInteroperableAssetThumbnailSet
{

  GENERATED_BODY()

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|ThumbnailSet", EditAnywhere, BlueprintReadWrite)
  FString SmallThumbnail;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|ThumbnailSet", EditAnywhere, BlueprintReadWrite)
  FString MediumThumbnail;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|ThumbnailSet", EditAnywhere, BlueprintReadWrite)
  FString LargeThumbnail;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|ThumbnailSet", EditAnywhere, BlueprintReadWrite)
  bool IsEmpty;

  FEmergenceInteroperableAssetThumbnailSet() {};

  FEmergenceInteroperableAssetThumbnailSet( FString _SmallThumbnail, FString _MediumThumbnail, FString _LargeThumbnail, bool _IsEmpty ){

    SmallThumbnail = _SmallThumbnail;
    MediumThumbnail = _MediumThumbnail;
    LargeThumbnail = _LargeThumbnail;
    IsEmpty = _IsEmpty;
  
  }
  
};

USTRUCT(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
struct FEmergenceInteroperableAssetMetadata
{

  GENERATED_BODY()

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|Metadatum", EditAnywhere, BlueprintReadWrite)
  FString Name;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|Metadatum", EditAnywhere, BlueprintReadWrite)
  FString Description;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|Metadatum", EditAnywhere, BlueprintReadWrite)
  FString Creator;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset|Metadatum", EditAnywhere, BlueprintReadWrite)
  FString Owner;

  FEmergenceInteroperableAssetMetadata() {};

  FEmergenceInteroperableAssetMetadata( FString _Name, FString _Description, FString _Creator, FString _Owner ){

    Name = _Name;
    Description = _Description;
    Creator = _Creator;
    Owner = _Owner;
  
  }
  
};

USTRUCT(Category="JSON|EmergenceInteroperableAsset", BlueprintType)
struct FEmergenceInteroperableAsset
{

  GENERATED_BODY()

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset", EditAnywhere, BlueprintReadWrite)
  FEmergenceInteroperableAssetMetadata Metadata;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset", EditAnywhere, BlueprintReadWrite)
  FEmergenceInteroperableAssetAsset PrimaryAsset;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset", EditAnywhere, BlueprintReadWrite)
  FEmergenceInteroperableAssetThumbnailSet ThumbnailSet;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset", EditAnywhere, BlueprintReadWrite)
  TArray<FEmergenceInteroperableAssetAsset> Assets;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset", EditAnywhere, BlueprintReadWrite)
  TArray<UEmergenceInteroperableAssetSubType*> SubTypeObjects;

  UPROPERTY(Category="JSON|EmergenceInteroperableAsset", EditAnywhere, BlueprintReadWrite)
  FString Id;

  FEmergenceInteroperableAsset() {};

  FEmergenceInteroperableAsset( FEmergenceInteroperableAssetMetadata _Metadata, FEmergenceInteroperableAssetAsset _PrimaryAsset, FEmergenceInteroperableAssetThumbnailSet _ThumbnailSet, TArray<FEmergenceInteroperableAssetAsset> _Assets, FString _Id ){

    Metadata = _Metadata;
    PrimaryAsset = _PrimaryAsset;
    ThumbnailSet = _ThumbnailSet;
    Assets = _Assets;
    Id = _Id;
  
  }

  FEmergenceInteroperableAsset(FString _json_) {
      UE_LOG(LogTemp, Display, TEXT("%s"), *_json_);
      FEmergenceInteroperableAsset _tmpEmergenceInteroperableAsset;

      FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInteroperableAsset>(_json_, &_tmpEmergenceInteroperableAsset, 0, 0);

      Metadata = _tmpEmergenceInteroperableAsset.Metadata;
      PrimaryAsset = _tmpEmergenceInteroperableAsset.PrimaryAsset;
      ThumbnailSet = _tmpEmergenceInteroperableAsset.ThumbnailSet;
      Assets = _tmpEmergenceInteroperableAsset.Assets;
      Id = _tmpEmergenceInteroperableAsset.Id;

      TSharedPtr<FJsonObject> JsonObject;
      auto Reader = TJsonReaderFactory<>::Create(_json_);
      if (FJsonSerializer::Deserialize(Reader, JsonObject))
      {
          TArray<TSharedPtr<FJsonValue>> SubTypesJSONValues;
          SubTypesJSONValues = JsonObject->GetArrayField("SubTypes");
          for (auto Value : SubTypesJSONValues) {
              if (Value->AsObject()->GetStringField("SubTypeName") == "NFT") {
                  UEmergenceInteroperableAssetNFTSubType* SubType = NewObject<UEmergenceInteroperableAssetNFTSubType>();
                  FJsonObjectConverter::JsonObjectToUStruct<FEmergenceInteroperableAssetNFTSubTypeInner>(Value->AsObject().ToSharedRef(), &SubType->EmergenceInteroperableAssetNFTSubType, 0, 0);
                  SubTypeObjects.Add(SubType);
              }
          }
      }
  };
};

UCLASS()
class EMERGENCE_API UEmergenceInteroperableAssetLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, meta=(DeterminesOutputType="SubtypeClass"))
    static UEmergenceInteroperableAssetSubType* GetInteroperableAssetSubType(TArray<UEmergenceInteroperableAssetSubType*> Array, UPARAM(meta = (AllowAbstract = false))TSubclassOf<UEmergenceInteroperableAssetSubType> SubtypeClass, bool& success);

    UFUNCTION(BlueprintPure)
    static FEmergenceInteroperableAsset GetTestIA();
};