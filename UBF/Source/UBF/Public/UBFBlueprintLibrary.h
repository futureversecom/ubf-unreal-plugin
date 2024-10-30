// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UBFAPISubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UBFBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UBF_API UUBFBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContext"))
	static void FindGraphsWithMatchingCategory(UObject* WorldContext, const FString& CategoryName, TArray<UUBFGraphReference*>& FoundGraphs);
};
