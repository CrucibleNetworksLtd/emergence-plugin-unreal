// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class EmergenceCustodial : ModuleRules
{
	public EmergenceCustodial(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

        PublicIncludePaths.Add(Path.GetFullPath(Path.Combine(PluginDirectory, "/ThirdParty/PicoSha2/...")));
		
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
				"JwtVerifier",
				"HashSHA256"
			}
		);

		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"SlateCore",
				"Json"
			}
		);
	}
}
