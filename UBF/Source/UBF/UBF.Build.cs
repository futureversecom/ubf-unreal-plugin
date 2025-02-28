// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using System.Runtime.InteropServices;
using UnrealBuildTool;

public class UBF : ModuleRules
{
	public UBF(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		if(Target.Platform == UnrealTargetPlatform.Win64){
			RuntimeDependencies.Add("$(TargetOutputDir)/../../Plugins/UBF/UBF/Binaries/Win64/ubf_interpreter.lib");
			RuntimeDependencies.Add("$(TargetOutputDir)/../../Plugins/UBF/UBF/Binaries/Win64/ubf_interpreter.dll");
		}
		
		PublicIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModuleDirectory, "Lib"),
                Path.Combine(ModuleDirectory, "Nodes")
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "glTFRuntime",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"HTTP",
				"Json",
				"DeveloperSettings"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);


		// Add any import libraries or static libraries
		PublicAdditionalLibraries.AddRange(
			new string[]
			{
                Path.Combine(ModuleDirectory, "Lib/ubf_interpreter.lib")
			}
		);
    }
}
