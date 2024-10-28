// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UBF : ModuleRules
{
	public UBF(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
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
				"FunctionalTesting",
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

		RuntimeDependencies.Add(
			"$(BinaryOutputDir)/ubf_interpreter.lib",
			Path.Combine(ModuleDirectory, "Lib/ubf_interpreter.lib")
		);

		RuntimeDependencies.Add(
			"$(BinaryOutputDir)/ubf_interpreter.dll",
			Path.Combine(ModuleDirectory, "Lib/ubf_interpreter.dll")
		);
    }
}
