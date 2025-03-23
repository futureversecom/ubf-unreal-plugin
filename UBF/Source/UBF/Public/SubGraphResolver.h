// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dynamic.h"
#include "UObject/Interface.h"

class UBF_API ISubGraphResolver
{
public:
	virtual bool TryResolveSubGraph(
		FString Key, const TMap<FString, UBF::FDynamicHandle>& ParentGraphInputs,
		FString& BlueprintId, TMap<FString, UBF::FDynamicHandle>& ResolvedInputs) = 0;
	
	virtual ~ISubGraphResolver() {}
};

class FDefaultSubGraphResolver final : public ISubGraphResolver
{
	virtual bool TryResolveSubGraph(
		FString Key, const TMap<FString, UBF::FDynamicHandle>& ParentGraphInputs,
		FString& BlueprintId, TMap<FString, UBF::FDynamicHandle>& ResolvedInputs) override
	{
		BlueprintId = Key;
		ResolvedInputs = ParentGraphInputs;
		return false;
	}
};
