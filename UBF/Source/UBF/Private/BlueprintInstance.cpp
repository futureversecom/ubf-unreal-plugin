#include "BlueprintInstance.h"

#include "UBFUtils.h"

void FBlueprintInstance::GetVariables(TMap<FString, UBF::FDynamicHandle>& OutVariables) const
{
	for (auto BindingTuple : Bindings)
	{
		OutVariables.Add(BindingTuple.Key, UBFUtils::CreateNewDynamicHandle(BindingTuple.Value.Type, BindingTuple.Value.Value));
	}
}
