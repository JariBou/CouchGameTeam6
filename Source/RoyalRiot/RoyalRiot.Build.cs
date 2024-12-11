// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RoyalRiot : ModuleRules
{
	public RoyalRiot(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "Niagara" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "LocalMultiplayerPlugin",
			"DeveloperSettings", "CameraSystemPlugin", "Niagara", "UMG"
		});
	}
}
