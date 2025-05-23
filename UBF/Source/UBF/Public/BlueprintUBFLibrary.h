// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UBFRuntimeController.h"
#include "ExecutionSets/ExecutionSetHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintUBFLibrary.generated.h"

class UUBFBindingObject;

namespace UBF
{
	class IExecutionSetData;

	UBF_API FExecutionSetHandle Execute(const FString& RootId, const TSharedPtr<const IExecutionSetData>& ExecutionSetData);
}

/**
 * 
 */
UCLASS()
class UBF_API UBlueprintUBFLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static UUBFBindingObject* CreateNewInputBindingObject(const FString& Id, const FString& Type, const FString& Value);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void RegisterCatalogs(const UObject* WorldContextObject, const FString& CatalogPath);
	
	UFUNCTION(BlueprintCallable)
	static void ExecuteBlueprint(UUBFRuntimeController* RuntimeController, const FString& BlueprintId,
	const TMap<FString, UUBFBindingObject*>& InputMap, const FOnComplete& OnComplete);
};
