// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFRuntimeController.h"

#include "GraphProvider.h"
#include "UBFBindingObject.h"
#include "UBFLog.h"

void UUBFRuntimeController::ExecuteGraph(FString GraphId, const TMap<FString, UUBFBindingObject*>& InputMap, const FOnComplete& OnComplete)
{
	if (!ensure(RootComponent)) return;

	UE_LOG(LogUBF, VeryVerbose, TEXT("UUBFRuntimeController::ExecuteGraph: %s Num Inputs: %d"), *GraphId, InputMap.Num());
	
	TMap<FString, UBF::FDynamicHandle> Inputs;

	for (const auto& InputPair : InputMap)
	{
		UE_LOG(LogUBF, Verbose, TEXT("Adding Input: %s"), *InputPair.Value->ToString());
		Inputs.Add(InputPair.Key, InputPair.Value->GetDynamicFromValue());
	}
	
	TryExecute(GraphId, Inputs, CurrentGraphProvider, CurrentSubGraphResolver, LastExecutionContext, OnComplete);
}

void UUBFRuntimeController::TryExecute(const FString& GraphId, const TMap<FString, UBF::FDynamicHandle>& Inputs,
		IGraphProvider* GraphProvider,  ISubGraphResolver* SubGraphResolver,
		UBF::FExecutionContextHandle& ExecutionContext, const FOnComplete& OnComplete) const
{
	if (!ensure(RootComponent)) return;
	UE_LOG(LogUBF, VeryVerbose, TEXT("UUBFRuntimeController::TryExecute"));

	if (GraphProvider == nullptr || SubGraphResolver == nullptr)
	{
		UE_LOG(LogUBF, Error, TEXT("Aborting execution: Invalid Graphprovider or SubGraphResolver provided!"));
		return;
	}
		
	CurrentGraphProvider = GraphProvider;
	CurrentSubGraphResolver = SubGraphResolver;
	
	CurrentGraphProvider->GetGraph(GraphId, GraphId).Next([this, GraphId, Inputs, &ExecutionContext, OnComplete](const UBF::FLoadGraphResult& Result)
	{
		if (!Result.Result.Key)
		{
			UE_LOG(LogUBF, Error, TEXT("Aborting execution: graph '%s' is invalid"), *GraphId);
			return;
		}

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

		auto OnCompleteFunc = [OnComplete]
		{
			OnComplete.ExecuteIfBound();
		};

		const UBF::FGraphHandle Graph = Result.Result.Value;
		Graph.Execute(GraphId, RootComponent, CurrentGraphProvider, CurrentSubGraphResolver, Inputs, OnCompleteFunc, ExecutionContext);
		UE_LOG(LogUBF, VeryVerbose, TEXT("UUBFRuntimeController::TryExecute Post Graph.Execute"));
	});
}

void UUBFRuntimeController::SetGraphProviders(IGraphProvider* GraphProvider, ISubGraphResolver* SubGraphResolver)
{
	CurrentGraphProvider = GraphProvider;
	CurrentSubGraphResolver = SubGraphResolver;
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
