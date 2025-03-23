// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "UBFEditor.h"

#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "UBFLog.h"
#include "UBFMaterialSettings.h"

#define LOCTEXT_NAMESPACE "FUBFEditorModule"

void FUBFEditorModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

		// Register the settings
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "UBF",
			LOCTEXT("RuntimeEditorSettingsName", "UBF"),
			LOCTEXT("RuntimeEditorSettingsDescription", "Settings for the UBF plugin."),
			GetMutableDefault<UUBFMaterialSettings>()
			);
	
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FUBFEditorModule::HandleSettingsChanged);
		}
	}
}

void FUBFEditorModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "UBFEditor", "UBF");
	}
}

bool FUBFEditorModule::HandleSettingsChanged()
{
	UUBFMaterialSettings* Settings = GetMutableDefault<UUBFMaterialSettings>();
	const auto Paths = FPaths::ProjectPluginsDir() + TEXT("/UBF/UBF/Config/DefaultUBF.ini") ;
	Settings->SaveConfig(CPF_Config, *FConfigCacheIni::NormalizeConfigIniPath(Paths));
	UE_LOG(LogUBF, Log, TEXT("Saving UBF UUBFMaterialSettings to %s"), *FConfigCacheIni::NormalizeConfigIniPath(Paths));
	return true;
}

void FUBFEditorModule::StartupModule()
{
	RegisterSettings();
}

void FUBFEditorModule::ShutdownModule()
{
	UnregisterSettings();
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUBFEditorModule, UBFEditor)