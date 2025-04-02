// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "BlueprintInstance.h"

void UBF::FBlueprintInstance::AddInput(const FString& Key, const FDynamicHandle& Handle)
{
	Inputs.Add(Key, Handle);
}

void UBF::FBlueprintInstance::AddInputs(const TMap<FString, FDynamicHandle>& NewInputs)
{
	Inputs.Append(NewInputs);
}
