// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UT_Game : ModuleRules
{
    public UT_Game(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "NavigationSystem",
            "Niagara",
            "GameplayTasks",

            // Networking and Online
            "Sockets",
            "Networking",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "OnlineSubsystemSteam",
            "Steamworks",
            "SteamSockets" // <-- critical: enables Steam P2P net driver
        });

        // Optional: Enables replication with Steam networking optimizations
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Json",
            "JsonUtilities"
        });

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string SteamPath = Path.Combine(EngineDirectory, "Binaries/ThirdParty/Steamworks/Steamv157/Win64");
            RuntimeDependencies.Add(Path.Combine(SteamPath, "steam_api64.dll"));
        }
    }
}
