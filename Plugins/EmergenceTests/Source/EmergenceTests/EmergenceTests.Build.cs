// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

using UnrealBuildTool;

public class EmergenceTests : ModuleRules
{
	public EmergenceTests(ReadOnlyTargetRules Target) : base(Target)
	{	
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"FunctionalTesting",
				"Emergence",
				"EmergenceFutureverseAssetRegistry",
				"Json",
                "JsonUtilities"
            }
		);
    }
}
