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
			Mapping.Add(FEmergenceVRMBonemapData( Asset->GetStringFromPath(BonePath, _), Asset->GetIntegerFromPath(NodePath, _)));
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
