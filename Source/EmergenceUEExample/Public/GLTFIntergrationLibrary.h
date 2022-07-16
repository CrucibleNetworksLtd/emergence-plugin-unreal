// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "glTFRuntimeAsset.h"
#include "GLTFIntergrationLibrary.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceVRMBoneMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString hips;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftUpperLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightUpperLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftLowerLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightLowerLeg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftFoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightFoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString spine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString chest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString neck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftShoulder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightShoulder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftUpperArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightUpperArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftLowerArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightLowerArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftToes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightToes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftEye;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightEye;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftThumbProximal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftThumbIntermediate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftThumbDistal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftIndexProximal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftIndexIntermediate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftIndexDistal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftMiddleProximal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftMiddleIntermediate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftMiddleDistal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftRingProximal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftRingIntermediate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftRingDistal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftLittleProximal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftLittleIntermediate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString leftLittleDistal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightThumbProximal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightThumbIntermediate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightThumbDistal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightIndexProximal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightIndexIntermediate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightIndexDistal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightMiddleProximal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightMiddleIntermediate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightMiddleDistal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightRingProximal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightRingIntermediate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightRingDistal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightLittleProximal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightLittleIntermediate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString rightLittleDistal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString upperChest;

};

USTRUCT(BlueprintType)
struct FEmergenceVRMBonemapData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FglTFRuntimeNode node;

	FEmergenceVRMBonemapData(FString _BoneName, FglTFRuntimeNode _node) {
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

	UFUNCTION(BlueprintPure)
	static TMap<FString, FString> GenerateMap(UglTFRuntimeAsset* Asset, FEmergenceVRMBoneMap BoneMap);
};
