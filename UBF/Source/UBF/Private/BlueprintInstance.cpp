#include "BlueprintInstance.h"

#include "UBFUtils.h"

TMap<FString, UBF::FDynamicHandle> FBlueprintInstance::GetVariables() const
{
	TMap<FString, UBF::FDynamicHandle> OutVariables;
	for (auto BindingTuple : Bindings)
	{
		OutVariables.Add(BindingTuple.Key, UBFUtils::CreateNewDynamicHandle(BindingTuple.Value.Type, BindingTuple.Value.Value));
	}
	return OutVariables;
}
