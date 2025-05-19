#pragma once

#include "ExecutionInstanceData.h"
#include "IExecutionSetConfig.h"

namespace UBF
{
	class UBF_API FExecutionSetConfig : public IExecutionSetConfig
	{
	public:
		FExecutionSetConfig(const TArray<FExecutionInstanceData>& ExecutionInstanceDatas, const TSharedPtr<FSceneNode>& RootNode);
		
		virtual void SetRootId(const FString& RootId) override;
		
		virtual TFuture<FLoadExecutionInstanceResult> GetExecutionInstance(const FString& Id) override;
		virtual TFuture<FLoadMeshResult> GetMesh(const FString& ArtifactId, const FMeshImportSettings& MeshImportSettings) override;
		virtual TFuture<FLoadTextureResult> GetTexture(const FString& ArtifactId) override;
		virtual TSharedPtr<FSceneNode> GetRoot() override;
		virtual TSharedPtr<FUBFLogData> GetLogData() override;
		virtual bool GetCancelExecution() override;
		virtual void FlagCancelExecution() override;
		
	private:
		UWorld* GetWorld() const;

		TMap<FString, TSharedPtr<FExecutionInstance>> ExecutionInstances;
		TMap<FString, FExecutionInstanceData> ExecutionInstanceDataMap;
		TSharedPtr<FSceneNode> RootNode = nullptr;
		TSharedPtr<FUBFLogData> LogData;
		bool bCancelExecutionRequested = false;
		FString RootBlueprintId;
	};
}
