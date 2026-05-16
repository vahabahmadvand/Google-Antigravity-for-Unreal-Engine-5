#include "AntigravitySourceCodeAccessModule.h"
#include "Features/IModularFeatures.h"
#include "Modules/ModuleManager.h"
#include "AntigravitySourceCodeAccessor.h"

LLM_DEFINE_TAG(AntigravitySourceCodeAccess);

IMPLEMENT_MODULE(FAntigravitySourceCodeAccessModule, AntigravitySourceCodeAccess);

#define LOCTEXT_NAMESPACE "AntigravitySourceCodeAccessor"

FAntigravitySourceCodeAccessModule::FAntigravitySourceCodeAccessModule()
	: AntigravitySourceCodeAccessor(MakeShareable(new FAntigravitySourceCodeAccessor()))
{
}

void FAntigravitySourceCodeAccessModule::StartupModule()
{
	AntigravitySourceCodeAccessor->Startup();

	// Bind our source control provider to the editor
	IModularFeatures::Get().RegisterModularFeature(TEXT("SourceCodeAccessor"), &AntigravitySourceCodeAccessor.Get());
}

void FAntigravitySourceCodeAccessModule::ShutdownModule()
{
	// unbind provider from editor
	IModularFeatures::Get().UnregisterModularFeature(TEXT("SourceCodeAccessor"), &AntigravitySourceCodeAccessor.Get());
	AntigravitySourceCodeAccessor->Shutdown();
}

FAntigravitySourceCodeAccessor& FAntigravitySourceCodeAccessModule::GetAccessor()
{
	return AntigravitySourceCodeAccessor.Get();
}

#undef LOCTEXT_NAMESPACE