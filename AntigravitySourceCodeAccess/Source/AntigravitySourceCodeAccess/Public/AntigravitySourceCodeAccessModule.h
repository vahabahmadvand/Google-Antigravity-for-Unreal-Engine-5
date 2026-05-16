#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FAntigravitySourceCodeAccessor;

class FAntigravitySourceCodeAccessModule : public IModuleInterface
{
public:

	FAntigravitySourceCodeAccessModule();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	FAntigravitySourceCodeAccessor& GetAccessor();

private:
	TSharedRef<FAntigravitySourceCodeAccessor> AntigravitySourceCodeAccessor;
};
