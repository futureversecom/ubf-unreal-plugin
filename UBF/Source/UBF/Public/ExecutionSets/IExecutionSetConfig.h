#pragma once
#include "GraphProvider.h"

namespace UBF
{
	class FExecutionInstance;
	class IExecutionSetConfig
	{
	public:
		virtual ~IExecutionSetConfig() = default;
		
		virtual TSharedPtr<FExecutionInstance> GetExecutionInstance(const FString& Id) = 0;
		virtual TFuture<FLoadMeshResult> GetMesh(const FString& ArtifactId) = 0;
		virtual TFuture<FLoadTextureResult> GetTexture(const FString& ArtifactId) = 0;
		
		virtual FSceneNode* GetRoot() = 0;
		virtual TSharedPtr<FUBFLogData> GetLogData() = 0;
		virtual bool GetCancelExecution() = 0;
	};
}

