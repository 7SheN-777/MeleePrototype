// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MeleePrototype : ModuleRules
{
	public MeleePrototype(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
            "AIModule",
			"NavigationSystem",
			"GameplayTasks",
            "StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"MeleePrototype",
			"MeleePrototype/Variant_Platforming",
			"MeleePrototype/Variant_Platforming/Animation",
			"MeleePrototype/Variant_Combat",
			"MeleePrototype/Variant_Combat/AI",
			"MeleePrototype/Variant_Combat/Animation",
			"MeleePrototype/Variant_Combat/Gameplay",
			"MeleePrototype/Variant_Combat/Interfaces",
			"MeleePrototype/Variant_Combat/UI",
			"MeleePrototype/Variant_SideScrolling",
			"MeleePrototype/Variant_SideScrolling/AI",
			"MeleePrototype/Variant_SideScrolling/Gameplay",
			"MeleePrototype/Variant_SideScrolling/Interfaces",
			"MeleePrototype/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
