#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUBFEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
private:
    void RegisterSettings();
    void UnregisterSettings();
    bool HandleSettingsChanged();
};
