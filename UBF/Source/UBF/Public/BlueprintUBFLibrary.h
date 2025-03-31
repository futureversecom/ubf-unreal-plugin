// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExecutionSets/ExecutionSetHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintUBFLibrary.generated.h"

namespace UBF
{
	class IExecutionSetData;

	// TODO List
	// 4. Update UBFRuntimeController to use new interface
	// 5. Update FutureverseControllerLayer to use new interface for parsing graphs
	// 6. Update FutureverseControllerLayer to use new interface for render graphs

	static FExecutionSetHandle Execute(const FString& RootId, const TSharedPtr<const IExecutionSetData>& ExecutionSetData);
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
