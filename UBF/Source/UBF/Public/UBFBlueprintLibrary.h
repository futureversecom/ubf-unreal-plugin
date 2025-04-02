// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UBFBindingObject.h"
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
	static UUBFBindingObject* CreateNewInputBindingObject(const FString& Id, const FString& Type, const FString& Value);
};
