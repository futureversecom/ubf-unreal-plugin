#include "ExecutionSets/ExecutionSetConfig.h"

#include "UBFLogData.h"
#include "ExecutionSets/ExecutionInstance.h"
#include "ExecutionSets/ExecutionInstanceData.h"
#include "GlobalArtifactProvider/GlobalArtifactProviderSubsystem.h"

namespace UBF
{
	FExecutionSetConfig::FExecutionSetConfig(const TArray<FExecutionInstanceData>& ExecutionInstanceDatas, const TSharedPtr<FSceneNode>& RootNode) : RootNode(RootNode)
	{
		for (const auto& ExecutionInstanceData : ExecutionInstanceDatas)
		{
			ExecutionInstanceDataMap.Add(ExecutionInstanceData.GetInstanceId(), ExecutionInstanceData);
		}
	}

	void FExecutionSetConfig::SetRootId(const FString& RootId)
	{
		RootBlueprintId = RootId;
		LogData = MakeShared<FUBFLogData>(RootId);
	}

	TFuture<FLoadExecutionInstanceResult> FExecutionSetConfig::GetExecutionInstance(const FString& Id)
	{
		TSharedPtr<TPromise<FLoadExecutionInstanceResult>> Promise = MakeShareable(new TPromise<FLoadExecutionInstanceResult>());
		TFuture<FLoadExecutionInstanceResult> Future = Promise->GetFuture();
		
		// Check Existing FExecutionInstance
		if (ExecutionInstances.Contains(Id))
		{
			FLoadExecutionInstanceResult LoadResult;
			LoadResult.bSuccess = true;
			LoadResult.Value = ExecutionInstances[Id];
			Promise->SetValue(LoadResult);
			return Future;
		}

		// Check Existing FExecutionInstance
		if (ExecutionInstanceDataMap.Contains(Id))
		{
			const FExecutionInstanceData& InstanceData = ExecutionInstanceDataMap[Id];
			UGlobalArtifactProviderSubsystem::Get(GetWorld())->GetGraph(InstanceData.GetBlueprintId()).Next([Promise, InstanceData, Id](const FLoadGraphResult& LoadGraphResult)
			{
				FLoadExecutionInstanceResult LoadResult;

				if (!LoadGraphResult.bSuccess)
				{
					LoadResult.SetFailure();
					Promise->SetValue(LoadResult);
					return;
				}

				TSharedPtr<FExecutionInstance> ExecutionInstance = MakeShared<FExecutionInstance>(Id, LoadGraphResult.Value);
				ExecutionInstance->SetInputs(InstanceData.GetInputs());
			
				LoadResult.SetResult(ExecutionInstance);
				Promise->SetValue(LoadResult);
			});
			return Future;
		}
	
		// Return blank instance from graph provider
		UGlobalArtifactProviderSubsystem::Get(GetWorld())->GetGraph(Id).Next([Promise, Id](const FLoadGraphResult& LoadGraphResult)
		{
			FLoadExecutionInstanceResult LoadResult;

			if (!LoadGraphResult.bSuccess)
			{
				LoadResult.SetFailure();
				Promise->SetValue(LoadResult);
				return;
			}
			
			LoadResult.SetResult(MakeShared<FExecutionInstance>(Id, LoadGraphResult.Value));
			Promise->SetValue(LoadResult);
		});

		return Future;
	}

	TFuture<FLoadMeshResult> FExecutionSetConfig::GetMesh(const FString& ArtifactId, const FMeshImportSettings& MeshImportSettings)
	{
		return UGlobalArtifactProviderSubsystem::Get(GetWorld())->GetMeshResource(ArtifactId, MeshImportSettings);
	}

	TFuture<FLoadMeshLODResult> FExecutionSetConfig::GetMeshLOD(const TArray<FString>& ArtifactIds,
		const FMeshConfigData& MeshConfigData)
	{
		return UGlobalArtifactProviderSubsystem::Get(GetWorld())->GetMeshLODResource(ArtifactIds, MeshConfigData);
	}

	TFuture<FLoadTextureResult> FExecutionSetConfig::GetTexture(const FString& ArtifactId)
	{
		return UGlobalArtifactProviderSubsystem::Get(GetWorld())->GetTextureResource(ArtifactId);
	}

	void FExecutionSetConfig::RegisterRuntimeCatalog(const FCatalogElement& CatalogElement)
	{
		UGlobalArtifactProviderSubsystem::Get(GetWorld())->RegisterCatalog(CatalogElement);
	}

	TSharedPtr<FSceneNode> FExecutionSetConfig::GetRoot()
	{
		return RootNode;
	}

	TSharedPtr<FUBFLogData> FExecutionSetConfig::GetLogData()
	{
		return LogData;
	}

	bool FExecutionSetConfig::GetCancelExecution()
	{
		return bCancelExecutionRequested;
	}

	void FExecutionSetConfig::FlagCancelExecution()
	{
		bCancelExecutionRequested = true;
	}

	UWorld* FExecutionSetConfig::GetWorld() const
	{
		return RootNode->GetAttachmentComponent()->GetWorld();
	}
}

