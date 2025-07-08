// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Graph.h"
#include "ExecutionSets/ExecutionInstance.h"
#include "UObject/Interface.h"

struct FglTFRuntimeConfig;
class UUBFGraphReference;
class UglTFRuntimeAsset;

namespace UBF
{
	template<typename T>
	struct UBF_API TUBFLoadResult
	{
		bool bSuccess = false;
		T Value;

		void SetResult(const T& InValue)
		{
			bSuccess = true;
			Value = InValue;
		}

		void SetFailure()
		{
			bSuccess = false;
		}
	};
	
	struct UBF_API FLoadGraphResult final : TUBFLoadResult<FGraphHandle> {};
	
	struct UBF_API FLoadExecutionInstanceResult final : TUBFLoadResult<TSharedPtr<FExecutionInstance>> {};
	
	struct UBF_API FLoadTextureResult final : TUBFLoadResult<UTexture*> {};

	struct UBF_API FLoadMeshResult final : TUBFLoadResult<UglTFRuntimeAsset*> {};
	
	struct UBF_API FLoadMeshLODResult final : TUBFLoadResult<UStreamableRenderAsset*> {};
	
	struct UBF_API FLoadStringResult final : TUBFLoadResult<FString> {};
	
	struct UBF_API FLoadDataArrayResult final : TUBFLoadResult<TSharedPtr<TArray<uint8>>> {};

	template struct TUBFLoadResult<TSharedPtr<TArray<uint8>>>;
}

class UBF_API IGraphProvider
{
public:
	virtual void PrintBlueprintDebug(const FString& ArtifactId, const FString& ContextString = FString()) = 0;
	
	virtual TFuture<UBF::FLoadGraphResult> GetGraph(const FString& ArtifactId) = 0;
	
	virtual TFuture<UBF::FLoadTextureResult> GetTextureResource(const FString& ArtifactId) = 0;
	
	virtual TFuture<UBF::FLoadMeshResult> GetMeshResource(const FString& ArtifactId, const FglTFRuntimeConfig& Config) = 0;

	virtual ~IGraphProvider() {}
};