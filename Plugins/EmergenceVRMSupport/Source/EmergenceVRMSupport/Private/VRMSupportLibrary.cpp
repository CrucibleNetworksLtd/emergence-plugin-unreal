// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "VRMSupportLibrary.h"
#include "LoaderBPFunctionLibrary.h"

void UVRMSupportLibrary::LoadVRMFileFromMemory(const UVrmAssetListObject* InVrmAsset, UVrmAssetListObject*& OutVrmAsset, TArray<uint8> Data, const FImportOptionData& OptionForRuntimeLoad) {
	VRMConverter::Options::Get().SetVrmOption(&OptionForRuntimeLoad);
	
	bool success = ULoaderBPFunctionLibrary::LoadVRMFileFromMemory(InVrmAsset, OutVrmAsset, "C:\\thing.vrm", Data.GetData(), Data.Num());
	UE_LOG(LogTemp, Warning, TEXT("Was a success: %s"), success ? "True" : "False");
}