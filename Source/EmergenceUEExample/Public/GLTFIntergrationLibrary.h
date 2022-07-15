// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "glTFRuntimeAsset.h"
#include "GLTFIntergrationLibrary.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceVRMBonemapData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int node;

	FEmergenceVRMBonemapData(FString _BoneName, int _node) {
		node = _node;
		BoneName = _BoneName;
	};

	FEmergenceVRMBonemapData() {};


};

UCLASS()
class EMERGENCEUEEXAMPLE_API UGLTFIntergrationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static bool GetBoneData(UglTFRuntimeAsset* Asset, TArray<FEmergenceVRMBonemapData>& Mapping);

	static FglTFRuntimePathItem MakeGlTFRuntimePathItem(FString Path, int32 Index);
};
