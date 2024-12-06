#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolModuleModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

protected:
    void RegisterMenuExtensions();
};
