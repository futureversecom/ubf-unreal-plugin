// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFRuntimeController.h"

#include <UBFLogData.h>
#include "GraphProvider.h"
#include "UBFBindingObject.h"
#include "UBFLog.h"
#include "DataTypes/MeshRenderer.h"

void UUBFRuntimeController::ExecuteBlueprint(FString BlueprintId, const FBlueprintExecutionData& ExecutionData,  const FOnComplete& OnComplete)
{
	if (!ensure(RootComponent))
	{
		OnComplete.ExecuteIfBound(false, FUBFExecutionReport::Failure());
		return;
	}

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
	LastExecutionContext.FlagCancelExecution();
	
	for (const auto AttachChild : GetSpawnedActors())
	{
		AttachChild->Destroy();
	}
}

void UUBFRuntimeController::TryExecute(const FString& BlueprintId, const TMap<FString, UBF::FDynamicHandle>& Inputs,
                                       const TSharedPtr<IGraphProvider>& GraphProvider,  const TMap<FString, UBF::FBlueprintInstance>& BlueprintInstances,
                                       UBF::FExecutionContextHandle& ExecutionContext, const FOnComplete& OnComplete)
{
	if (!ensure(RootComponent))
	{
		OnComplete.ExecuteIfBound(false, FUBFExecutionReport::Failure());
		return;
	}
	
	UE_LOG(LogUBF, VeryVerbose, TEXT("UUBFRuntimeController::TryExecute"));

	if (GraphProvider == nullptr)
	{
		UE_LOG(LogUBF, Error, TEXT("Aborting execution: Invalid Graphprovider or SubGraphResolver provided!"));
		OnComplete.ExecuteIfBound(false, FUBFExecutionReport::Failure());
		return;
	}
		
	CurrentGraphProvider = GraphProvider;
	
	CurrentGraphProvider->GetGraph(BlueprintId).Next([this, BlueprintId, Inputs, &ExecutionContext, OnComplete, BlueprintInstances](const UBF::FLoadGraphResult& Result)
	{
		if (!IsValid(this) || !IsValid(GetWorld()) || GetWorld()->bIsTearingDown)
		{
			this->OnComplete(false);
			OnComplete.ExecuteIfBound(false, FUBFExecutionReport::Failure());
			return;
		}
		
		if (!Result.Result.Key)
		{
			UE_LOG(LogUBF, Error, TEXT("Aborting execution: graph '%s' is invalid"), *BlueprintId);
			this->OnComplete(false);
			OnComplete.ExecuteIfBound(false, FUBFExecutionReport::Failure());
			return;
		}

		ClearBlueprint();
		if (bStartWithUBFActorsHidden)
			SetUBFActorsHidden(true);

		auto OnCompleteFunc = [OnComplete, this](bool Success, FUBFExecutionReport ExecutionReport)
		{
			this->OnComplete(Success);
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

bool UUBFRuntimeController::TryReadLastContextOutput(const FString& OutputId, FString& OutString) const
{
	bool bSuccess = false;
	
	UBF::FDynamicHandle DynamicOutput;
	if (LastExecutionContext.TryReadOutput(OutputId, DynamicOutput))
	{
		bSuccess = true;
		OutString = DynamicOutput.ToString();
	}
	
	return bSuccess;
}

UObject* UUBFRuntimeController::TryReadLastContextUObjectOutput(const FString& OutputId) const
{
	UBF::FDynamicHandle DynamicOutput;
	if (!LastExecutionContext.TryReadOutput(OutputId, DynamicOutput))
	{
		return nullptr;
	}
	
	UBF::FSceneNode* SceneNode = nullptr;
	if (DynamicOutput.TryInterpretAs(SceneNode) && SceneNode)
	{
		return SceneNode->GetAttachmentComponent();
	}
	
	UBF::FMeshRenderer* MeshRenderer = nullptr;
	if (DynamicOutput.TryInterpretAs(MeshRenderer) && MeshRenderer)
	{
		return MeshRenderer->GetMesh();
	}

	return nullptr;
}

void UUBFRuntimeController::BeginPlay()
{
	Super::BeginPlay();

	if (RootComponent == nullptr)
		RootComponent = GetOwner()->GetRootComponent();
}

void UUBFRuntimeController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearBlueprint();
	
	Super::EndPlay(EndPlayReason);
}

void UUBFRuntimeController::SetUBFActorsHidden(bool bIsHidden)
{
	RootComponent->SetVisibility(!bIsHidden);

	for (AActor* Actor : GetSpawnedActors())
	{
		Actor->SetActorHiddenInGame(bIsHidden);
	}
}

void UUBFRuntimeController::OnComplete(bool bWasSuccessful)
{
	if (!IsValid(this)) return;

	if (bWasSuccessful && bAutoUnHideUBFActorsOnComplete)
		SetUBFActorsHidden(false);
}

TArray<AActor*> UUBFRuntimeController::GetSpawnedActors() const
{
	TArray<AActor*> ChildActors;
	for (const auto AttachChild : RootComponent->GetAttachChildren())
	{
		if (AttachChild->GetOwner() != RootComponent->GetOwner())
		{
			ChildActors.Add(AttachChild->GetOwner());
			AttachChild->GetOwner()->GetAttachedActors(ChildActors, false, true);
		}
	}

	return ChildActors;
}
