// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "UBFRuntimeController.h"

#include "UBFLogData.h"

#include "BlueprintUBFLibrary.h"
#include "UBFUtils.h"
#include "DataTypes/MeshRenderer.h"
#include "ExecutionSets/ExecutionSetData.h"
#include "ExecutionSets/ExecutionSetResult.h"

void UUBFRuntimeController::ExecuteBlueprint(FString RootID, const FBlueprintExecutionData& ExecutionData,  const FOnComplete& OnComplete)
{
	if (!ensure(RootComponent))
	{
		OnComplete.ExecuteIfBound(false, FUBFExecutionReport::Failure());
		return;
	}
	
	ClearBlueprint();
	if (bStartWithUBFActorsHidden)
		SetUBFActorsHidden(true);

	auto OnCompleteFunc = [OnComplete, this](bool Success, TSharedPtr<UBF::FExecutionSetResult> ExecutionSetResult)
	{
		this->OnComplete(Success);

		/* fixes race condition from UBF::Execute finishing instantly */
		if (!LastSetHandle.GetResult().IsValid())
		{
			LastSetHandle = UBF::FExecutionSetHandle(nullptr, ExecutionSetResult);
		}
		
		OnComplete.ExecuteIfBound(Success, ExecutionSetResult->GetExecutionReport());
	};

	TSharedPtr<UBF::FExecutionSetData> ExecutionSetData = MakeShared<UBF::FExecutionSetData>(MakeShared<UBF::FSceneNode>(RootComponent), ExecutionData.BlueprintInstances, MoveTemp(OnCompleteFunc));

	ExecutionSetData->AddInputsForId(RootID, UBFUtils::AsDynamicMap(ExecutionData.InputMap));
	
	LastSetHandle = UBF::Execute(RootID, ExecutionSetData);
}

void UUBFRuntimeController::ClearBlueprint()
{
	LastSetHandle.FlagShouldCancel();
	
	for (const auto AttachChild : GetSpawnedActors())
	{
		AttachChild->Destroy();
	}
}

TArray<FString> UUBFRuntimeController::GetLastOutputNames()
{
	if (!LastSetHandle.IsValid()) return TArray<FString>();
	
	return LastSetHandle.GetResult()->GetOutputNames();
}

bool UUBFRuntimeController::TryReadLastContextOutput(const FString& OutputId, FString& OutString) const
{
	if (!LastSetHandle.IsValid()) return false;
	
	return LastSetHandle.GetResult()->TryReadOutputString(OutputId, OutString);
}

UObject* UUBFRuntimeController::TryReadLastContextUObjectOutput(const FString& OutputId) const
{
	if (!LastSetHandle.IsValid()) return nullptr;
	
	UBF::FDynamicHandle DynamicOutput;
	if (!LastSetHandle.GetResult()->TryReadOutput(OutputId, DynamicOutput))
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
