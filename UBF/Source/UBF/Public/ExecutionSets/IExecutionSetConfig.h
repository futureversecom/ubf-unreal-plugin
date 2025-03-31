#pragma once
#include "GraphProvider.h"

struct FMeshImportSettings;

namespace UBF
{
	class FExecutionInstance;
	
	class IExecutionSetConfig
	{
	public:
		virtual ~IExecutionSetConfig() = default;
		
		virtual TFuture<FLoadExecutionInstanceResult> GetExecutionInstance(const FString& Id) = 0;
		virtual TFuture<FLoadMeshResult> GetMesh(const FString& ArtifactId, const FMeshImportSettings& MeshImportSettings) = 0;
		virtual TFuture<FLoadTextureResult> GetTexture(const FString& ArtifactId) = 0;
		
		virtual FSceneNode* GetRoot() = 0;
		virtual TSharedPtr<FUBFLogData> GetLogData() = 0;
		virtual bool GetCancelExecution() = 0;
		virtual void FlagCancelExecution() = 0;

		virtual void SetRootId(const FString& RootId) = 0;
	};
}

