// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "ExecutionSets/ExecutionInstanceData.h"

void UBF::FExecutionInstanceData::AddInput(const FString& Key, const FDynamicHandle& Handle)
{
	Inputs.Add(Key, Handle);
}

void UBF::FExecutionInstanceData::AddInputs(const TMap<FString, FDynamicHandle>& NewInputs)
{
	Inputs.Append(NewInputs);
}
