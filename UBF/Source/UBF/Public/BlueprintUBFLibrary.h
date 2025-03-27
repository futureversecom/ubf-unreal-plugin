// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintUBFLibrary.generated.h"

namespace UBF
{
	class IExecutionSetData;

	static void Execute(const FString& RootId, const TSharedPtr<IExecutionSetData>& ExecutionSetData);
}

/**
 * 
 */
UCLASS()
class UBF_API UBlueprintUBFLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
};
