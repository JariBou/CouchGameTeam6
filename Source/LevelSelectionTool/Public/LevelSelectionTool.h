#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FLevelSelectionToolModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

protected:
    void RegisterMenuExtensions();
};
