// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EmergenceUEExample : ModuleRules
{
	public EmergenceUEExample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		if (Target.Version.MajorVersion >= 5 && Target.Version.MinorVersion < 5)
		{
			CppStandard = CppStandardVersion.Cpp17;
		}
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay",
			"EmergenceCore",
            "EmergenceBlockchainWallet"
        });
	}
}
