#include "BlueprintInstance.h"

TMap<FString, UBF::FDynamicHandle>& FBlueprintInstance::GetVariablesRef() const
{
	TMap<FString, UBF::FDynamicHandle> Variables;
	
	// todo convert bindings into variables
	return Variables;
}
