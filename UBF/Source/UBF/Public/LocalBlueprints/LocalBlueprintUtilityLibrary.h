// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LocalBlueprintUtilityLibrary.generated.h"

USTRUCT(BlueprintType)
struct FLoadedBlueprintInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString BlueprintID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HumanReadableName;
};

/**
 * 
 */
UCLASS()
class UBF_API ULocalBlueprintUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/* Uses file explorer to get path to catalog json, blocks GT. Only supports editor or Windows */
	UFUNCTION(BlueprintCallable)
	static FString SelectPathFromFileExplorer(const FString& DefaultPath);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext=WorldContext))
	static void GetLoadedBlueprints(UObject* WorldContext, TArray<FLoadedBlueprintInfo>& LoadedBlueprintInfos);
};
