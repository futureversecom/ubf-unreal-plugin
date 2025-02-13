// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFRuntimeController.h"

#include <UBFLogData.h>
#include "GraphProvider.h"
#include "UBFBindingObject.h"
#include "UBFLog.h"

void UUBFRuntimeController::ExecuteBlueprint(FString BlueprintId, const FBlueprintExecutionData& ExecutionData,  const FOnComplete& OnComplete)
{
	if (!ensure(RootComponent)) return;

	UE_LOG(LogUBF, Verbose, TEXT("UUBFRuntimeController::ExecuteBlueprint: %s Num Inputs: %d Num BlueprintInstances: %d"), *BlueprintId, ExecutionData.InputMap.Num(), ExecutionData.BlueprintInstances.Num());
	
	TMap<FString, UBF::FDynamicHandle> Inputs;

	for (const auto& InputPair : ExecutionData.InputMap)
	{
		UE_LOG(LogUBF, Verbose, TEXT("UUBFRuntimeController::ExecuteBlueprint Adding Input: %s"), *InputPair.Value->ToString());
		Inputs.Add(InputPair.Key, InputPair.Value->GetDynamicFromValue());
	}

	TMap<FString, UBF::FBlueprintInstance> InstanceMap;

	for (const UBF::FBlueprintInstance& BlueprintInstance : ExecutionData.BlueprintInstances)
	{
		UE_LOG(LogUBF, Verbose, TEXT("UUBFRuntimeController::ExecuteBlueprint Adding BlueprintInstance: %s"), *BlueprintInstance.ToString());
		InstanceMap.Add(BlueprintInstance.GetInstanceId(), BlueprintInstance);
	}
	
	TryExecute(BlueprintId, Inputs, CurrentGraphProvider, InstanceMap, LastExecutionContext, OnComplete);
}

void UUBFRuntimeController::ClearBlueprint()
{
	TArray<TObjectPtr<USceneComponent>> Children = RootComponent->GetAttachChildren();
	for (const auto AttachChild : Children)
	{
		TArray<AActor*> AttachedActorsChildren;
		AttachChild->GetOwner()->GetAttachedActors(AttachedActorsChildren, false, true);
		for (const auto AttachedActorChild : AttachedActorsChildren)
		{
			AttachedActorChild->Destroy();
		}
				
		AttachChild->GetOwner()->Destroy();
	}

	// TODO cancel any current blueprint executions
}

void UUBFRuntimeController::TryExecute(const FString& BlueprintId, const TMap<FString, UBF::FDynamicHandle>& Inputs,
                                       const TSharedPtr<IGraphProvider>& GraphProvider,  const TMap<FString, UBF::FBlueprintInstance>& BlueprintInstances,
                                       UBF::FExecutionContextHandle& ExecutionContext, const FOnComplete& OnComplete)
{
	if (!ensure(RootComponent)) return;
	UE_LOG(LogUBF, VeryVerbose, TEXT("UUBFRuntimeController::TryExecute"));

	if (GraphProvider == nullptr)
	{
		UE_LOG(LogUBF, Error, TEXT("Aborting execution: Invalid Graphprovider or SubGraphResolver provided!"));
		return;
	}
		
	CurrentGraphProvider = GraphProvider;
	
	CurrentGraphProvider->GetGraph(BlueprintId).Next([this, BlueprintId, Inputs, &ExecutionContext, OnComplete, BlueprintInstances](const UBF::FLoadGraphResult& Result)
	{
		if (!Result.Result.Key)
		{
			UE_LOG(LogUBF, Error, TEXT("Aborting execution: graph '%s' is invalid"), *BlueprintId);
			return;
		}

		ClearBlueprint();

		auto OnCompleteFunc = [OnComplete](bool Success, FUBFExecutionReport ExecutionReport)
		{
			OnComplete.ExecuteIfBound(Success, ExecutionReport);
		};

		LastGraphHandle = Result.Result.Value;
		LastGraphHandle.Execute(BlueprintId, RootComponent, CurrentGraphProvider, MakeShared<FUBFLogData>(BlueprintId), BlueprintInstances, Inputs, OnCompleteFunc, ExecutionContext);
		UE_LOG(LogUBF, VeryVerbose, TEXT("UUBFRuntimeController::TryExecute Post Graph.Execute"));
	});
}

void UUBFRuntimeController::SetGraphProviders(const TSharedPtr<IGraphProvider>& GraphProvider)
{
	CurrentGraphProvider = GraphProvider;
}

TArray<FString> UUBFRuntimeController::GetLastOutputNames()
{
	TArray<UBF::FBindingInfo> Outputs;
	LastGraphHandle.GetOutputs(Outputs);
	TArray<FString> OutputNames;

	for (auto Output : Outputs)
	{
		OutputNames.Add(Output.Id);
	}

	return OutputNames;
}

void UUBFRuntimeController::BeginPlay()
{
	Super::BeginPlay();

	if (RootComponent == nullptr)
		RootComponent = GetOwner()->GetRootComponent();
}

void UUBFRuntimeController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
