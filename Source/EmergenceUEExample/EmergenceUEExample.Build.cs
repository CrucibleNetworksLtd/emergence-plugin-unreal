// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EmergenceUEExample : ModuleRules
{
	public EmergenceUEExample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay",
			"Emergence",
			"JsonUtilities",
			"Json"
		});
	}
}
