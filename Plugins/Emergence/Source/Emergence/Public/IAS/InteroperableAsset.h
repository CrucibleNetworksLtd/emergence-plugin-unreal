// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"
#include "IAS/NFTElement.h"
#include "IAS/AvatarElement.h"
#include "InteroperableAsset.generated.h"

USTRUCT(Category="Emergence|Interoperable Asset System", BlueprintType)
struct FEmergenceInteroperableAsset
{

  GENERATED_BODY()

  UPROPERTY(Category="Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
  TArray<UEmergenceInteroperableAssetElement*> Elements;

  UPROPERTY(Category="Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
  FString Id;

  FEmergenceInteroperableAsset() {};


  FEmergenceInteroperableAsset(FString _json_) {
      UE_LOG(LogTemp, Display, TEXT("%s"), *_json_);
      FEmergenceInteroperableAsset _tmpEmergenceInteroperableAsset;

      FJsonObjectConverter::JsonObjectStringToUStruct<FEmergenceInteroperableAsset>(_json_, &_tmpEmergenceInteroperableAsset, 0, 0);

      Id = _tmpEmergenceInteroperableAsset.Id;

      TSharedPtr<FJsonObject> JsonObject;
      auto Reader = TJsonReaderFactory<>::Create(_json_);
      if (FJsonSerializer::Deserialize(Reader, JsonObject))
      {
          TArray<TSharedPtr<FJsonValue>> ElementsJSONValues;
          ElementsJSONValues = JsonObject->GetArrayField("Elements");
          for (auto Value : ElementsJSONValues) {
              if (Value->AsObject()->GetStringField("ElementName") == "NFT") {
                  UEmergenceInteroperableAssetNFTElement* Element = NewObject<UEmergenceInteroperableAssetNFTElement>();
                  FJsonObjectConverter::JsonObjectToUStruct<FEmergenceInteroperableAssetNFTElementInner>(Value->AsObject().ToSharedRef(), &Element->EmergenceInteroperableAssetNFTElement, 0, 0);
                  Elements.Add(Element);
              }
              if (Value->AsObject()->GetStringField("ElementName") == "avatar") {
                  UEmergenceInteroperableAssetAvatarElement* Element = NewObject<UEmergenceInteroperableAssetAvatarElement>();
                  FJsonObjectConverter::JsonObjectToUStruct<FEmergenceInteroperableAssetAvatarElementInner>(Value->AsObject().ToSharedRef(), &Element->EmergenceInteroperableAssetAvatarElement, 0, 0);
                  Elements.Add(Element);
              }
          }
      }
  };
};