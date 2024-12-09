// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class ProjectTNH : ModuleRules
{
	public ProjectTNH(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine",
			"InputCore", "EnhancedInput", "UMG", "Niagara", "GameplayTasks", "NavigationSystem",
			"AIModule", "AnimGraphRuntime", "DidItHit", "MotionWarping", "Json", "JsonUtilities"});

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "MediaAssets" });

	}
}
