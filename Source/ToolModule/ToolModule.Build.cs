using UnrealBuildTool;

public class ToolModule : ModuleRules
{
    public ToolModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "RoyalRiot",
                "UMGEditor",
                "Blutility",
                "UMG",
                "Niagara",
                "Json",
                "JsonUtilities",
                "AssetTools",
                "UnrealEd"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "ToolMenus",
                "PropertyEditor",
                "Niagara"
            }
        );
    }
}