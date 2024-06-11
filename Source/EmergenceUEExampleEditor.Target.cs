// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class EmergenceUEExampleEditorTarget : TargetRules
{
	public EmergenceUEExampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.Add("EmergenceUEExample");
		ExtraModuleNames.Add("EmergenceTestEditor");
	}
}
