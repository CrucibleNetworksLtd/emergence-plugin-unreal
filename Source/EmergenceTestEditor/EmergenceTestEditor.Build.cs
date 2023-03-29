// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EmergenceTestEditor : ModuleRules
{
	public EmergenceTestEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"Emergence",
			"JsonUtilities",
			"Json",
			"UnrealEd",
			"BlueprintGraph"
		});
	}
}
