// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Emergence : ModuleRules
{
	public Emergence(ReadOnlyTargetRules Target) : base(Target)
	{
		bool MarketplaceBuild = true;
		
		if(MarketplaceBuild || Target.Platform == UnrealTargetPlatform.Mac){
			PublicDefinitions.Add("UNREAL_MARKETPLACE_BUILD=1");
		}
		else{
			PublicDefinitions.Add("UNREAL_MARKETPLACE_BUILD=0");
			RuntimeDependencies.Add("$(BinaryOutputDir)", "$(PluginDir)/EmergenceServer/Windows/...");
		}
		
		if(Target.Platform == UnrealTargetPlatform.Win64){
			RuntimeDependencies.Add("$(TargetOutputDir)/../../Plugins/Emergence/EmergenceDll/Win64/...");
		}
		
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
        //PublicIncludePaths.Add(Path.Combine(EngineDirectory, "/Source/ThirdParty/OpenSSL/1.1.1c/include/Win64/VS2015/..."));
        PublicIncludePaths.Add(Path.Combine(PluginDirectory, "/ThirdParty/..."));
        PublicIncludePaths.Add(Path.Combine(PluginDirectory, "/ThirdParty/jwt-cpp/..."));
		PublicDependencyModuleNames.Add("OpenSSL");
		
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
				"Projects",
				"HTTP",
				"HTTPServer",
				"JwtVerifier"
			}
		);

		if (Target.Version.MajorVersion >= 5 && Target.Version.MinorVersion >= 2)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"ApplicationCore"
				}
			);
		}	
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"SlateCore",
				"Json"
				// ... add private dependencies that you statically link with here ...	
			}
		);
	}
}
