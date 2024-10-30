// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFBlueprintLibrary.h"

void UUBFBlueprintLibrary::FindGraphsWithMatchingCategory(UObject* WorldContext, const FString& CategoryName,
	TArray<UUBFGraphReference*>& FoundGraphs)
{
	auto UbfSubsystem = UUBFAPISubsystem::Get(WorldContext);
	if (!UbfSubsystem) return;
	
	TArray<UUBFGraphReference*> LoadedGraphs;
	UbfSubsystem->GetLoadedGraphs(LoadedGraphs);

	for (auto LoadedGraph : LoadedGraphs)
	{
		if (LoadedGraph->GetBlueprintId().Contains(CategoryName))
		{
			FoundGraphs.Add(LoadedGraph);
		}
	}
}
