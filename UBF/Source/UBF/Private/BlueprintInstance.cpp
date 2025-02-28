#include "BlueprintInstance.h"

void UBF::FBlueprintInstance::AddInput(const FString& Key, const FDynamicHandle& Handle)
{
	Inputs.Add(Key, Handle);
}

void UBF::FBlueprintInstance::AddInputs(const TMap<FString, FDynamicHandle>& NewInputs)
{
	Inputs.Append(NewInputs);
}
