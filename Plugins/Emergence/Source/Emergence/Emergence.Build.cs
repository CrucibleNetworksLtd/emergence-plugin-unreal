// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

using UnrealBuildTool;

public class Emergence : ModuleRules
{
	public Emergence(ReadOnlyTargetRules Target) : base(Target)
	{
		RuntimeDependencies.Add("$(BinaryOutputDir)", "$(PluginDir)/EmergenceServer/...");
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"UMG",
				"Sockets",
				"Networking",
				"JsonUtilities",
				"Slate",
				"SlateCore",
				"Projects"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"SlateCore",
				"HTTP",
				"Json",
				// ... add private dependencies that you statically link with here ...	
			}
			);
	}
}
