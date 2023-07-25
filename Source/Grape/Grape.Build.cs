// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Grape : ModuleRules
{
	public Grape(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "OnlineSubsystem", "OnlineSubsystemEOS", "OnlineSubsystemUtils", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
