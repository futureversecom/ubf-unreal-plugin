// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintJson.h"
#include "Graph.h"
#include "ExecutionSets/ExecutionInstance.h"
#include "UObject/Interface.h"

struct FglTFRuntimeConfig;
class UUBFGraphReference;
class UglTFRuntimeAsset;

namespace UBF
{
	template<typename T>
	struct UBF_API TLoadResult
	{
		TPair<bool, T> Result;
	};
	
	struct UBF_API FLoadGraphResult final : TLoadResult<FGraphHandle> {};
	
	struct UBF_API FLoadExecutionInstanceResult final : TLoadResult<TSharedPtr<FExecutionInstance>> {};
	
	struct UBF_API FLoadGraphInstanceResult final : TLoadResult<FBlueprintJson> {};

	struct UBF_API FLoadTextureResult final : TLoadResult<UTexture*> {};

	struct UBF_API FLoadMeshResult final : TLoadResult<UglTFRuntimeAsset*> {};
	
	struct UBF_API FLoadStringResult final : TLoadResult<FString> {};
	
	struct UBF_API FLoadDataArrayResult final : TLoadResult<TArray<uint8>> {};
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