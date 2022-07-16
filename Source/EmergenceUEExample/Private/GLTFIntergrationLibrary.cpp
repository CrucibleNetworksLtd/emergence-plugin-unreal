// Fill out your copyright notice in the Description page of Project Settings.


#include "GLTFIntergrationLibrary.h"

bool UGLTFIntergrationLibrary::GetBoneData(UglTFRuntimeAsset* Asset, TArray<FEmergenceVRMBonemapData>& Mapping) {
	
	if(!Asset)
	{
	return false;
	}

	TArray<FglTFRuntimePathItem> BonesArrayPath;
	BonesArrayPath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("extensions", -1));
	BonesArrayPath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("VRM", -1));
	BonesArrayPath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("humanoid", -1));
	BonesArrayPath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("humanBones", -1));

	bool Found;
	int BonesNum = Asset->GetArraySizeFromPath(BonesArrayPath, Found);

	Mapping.Empty();

	if (Found) {
	for (int i = 0; i < BonesNum; i++) {
		TArray<FglTFRuntimePathItem> BonePath;
		BonePath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("extensions", -1));
		BonePath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("VRM", -1));
		BonePath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("humanoid", -1));
		BonePath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("humanBones", i));
		BonePath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("bone", -1));

		TArray<FglTFRuntimePathItem> NodePath;
		NodePath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("extensions", -1));
		NodePath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("VRM", -1));
		NodePath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("humanoid", -1));
		NodePath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("humanBones", i));
		NodePath.Add(UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem("node", -1));
		bool _;

		FglTFRuntimeNode Node;
		Asset->GetNode(Asset->GetIntegerFromPath(NodePath, _), Node);

		Mapping.Add(FEmergenceVRMBonemapData( Asset->GetStringFromPath(BonePath, _), Node));
	}
	return true;
	}
	else {
	return false;
	}
	
}

FglTFRuntimePathItem UGLTFIntergrationLibrary::MakeGlTFRuntimePathItem(FString Path, int32 Index)
{
	FglTFRuntimePathItem Item;
	Item.Path = Path;
	Item.Index = Index;
	return Item;
}

TMap<FString, FString> UGLTFIntergrationLibrary::GenerateMap(UglTFRuntimeAsset* Asset, FEmergenceVRMBoneMap BoneMap)
{
	TArray<FEmergenceVRMBonemapData> Mapping;
	UGLTFIntergrationLibrary::GetBoneData(Asset, Mapping);

	TMap<FString, FString> Map;

	for (TFieldIterator<UProperty> It(BoneMap.StaticStruct()); It; ++It)
	{
		UProperty* Property = *It;
		FString VariableName = CastField<FStrProperty>(Property)->GetName(); //VRM name
		FString ModelBoneName = Mapping.FindByPredicate([&](FEmergenceVRMBonemapData BoneMapData) { return BoneMapData.BoneName == VariableName; })->node.Name;
		FString VariableContent = CastField<FStrProperty>(Property)->GetPropertyValue_InContainer(&BoneMap); //UE4 Model name
		//UE_LOG(LogTemp, Warning, TEXT("VRM Bone: %s, GLTF Bone: %s, UE4 Skeleton Bone: %s"), *VariableName, *ModelBoneName, *VariableContent);
		if (!ModelBoneName.IsEmpty() && !VariableContent.IsEmpty())
		{
			Map.Add(ModelBoneName, VariableContent);
		}
	}

	return Map;
}
