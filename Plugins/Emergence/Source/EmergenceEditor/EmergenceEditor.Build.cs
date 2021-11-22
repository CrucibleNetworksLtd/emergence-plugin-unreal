// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EmergenceEditor : ModuleRules
{
	public EmergenceEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
			new string[] {
                "Core",
				"CoreUObject",
				"Engine", 
				"RHI"
            }
		);

		PublicDependencyModuleNames.AddRange(
		new string[] { 
			"Emergence" 
			}
		);
	}
	
}
