// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph.h"
#include "Registry.h"
#include "UObject/Interface.h"

class IBlueprintInstance;
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
	struct UBF_API FLoadGraphInstanceResult final : TLoadResult<IBlueprintInstance*> {};

	struct UBF_API FLoadTextureResult final : TLoadResult<UTexture*> {};

	struct UBF_API FLoadMeshResult final : TLoadResult<UglTFRuntimeAsset*> {};
	
	struct UBF_API FLoadStringResult final : TLoadResult<FString> {};
	
	struct UBF_API FLoadDataArrayResult final : TLoadResult<TArray<uint8>> {};
}

class UBF_API IGraphProvider
{
public:
	virtual TFuture<UBF::FLoadGraphResult> GetGraph(const FString& InstanceId) = 0;
	
	virtual TFuture<UBF::FLoadGraphInstanceResult> GetGraphInstance(const FString& InstanceId) = 0;
	
	virtual TFuture<UBF::FLoadTextureResult> GetTextureResource(const FString& InstanceId, const FString& ResourceId) = 0;
	
	virtual TFuture<UBF::FLoadDataArrayResult> GetMeshResource(const FString& InstanceId, const FString& ResourceId) = 0;

	virtual ~IGraphProvider() {}
};


class FDefaultGraphProvider final : public IGraphProvider
{
	
public:
	void RegisterGraph(const FString& Id, const UBF::FGraphHandle& Graph)
	{
		Graphs.Add(Id, Graph);
	}

	void RegisterInstance(const FString& Id, IBlueprintInstance* Instance)
	{
		Instances.Add(Id, Instance);
	}

	virtual TFuture<UBF::FLoadGraphResult> GetGraph(const FString& InstanceId) override
	{
		TSharedPtr<TPromise<UBF::FLoadGraphResult>> Promise = MakeShareable(new TPromise<UBF::FLoadGraphResult>());
		TFuture<UBF::FLoadGraphResult> Future = Promise->GetFuture();
		UBF::FLoadGraphResult LoadResult;

		LoadResult.Result = Graphs.Contains(InstanceId)
			? TPair<bool, UBF::FGraphHandle>(true, Graphs[InstanceId])
			: TPair<bool, UBF::FGraphHandle>(false, UBF::FGraphHandle());
	
		Promise->SetValue(LoadResult);
		
		return Future;
	}

	virtual TFuture<UBF::FLoadGraphInstanceResult> GetGraphInstance(const FString& InstanceId) override
	{
		TSharedPtr<TPromise<UBF::FLoadGraphInstanceResult>> Promise = MakeShareable(new TPromise<UBF::FLoadGraphInstanceResult>());
		TFuture<UBF::FLoadGraphInstanceResult> Future = Promise->GetFuture();
		UBF::FLoadGraphInstanceResult LoadResult;

		LoadResult.Result = Instances.Contains(InstanceId)
			? TPair<bool, IBlueprintInstance*>(true, Instances[InstanceId])
			: TPair<bool, IBlueprintInstance*>(false, nullptr);
	
		Promise->SetValue(LoadResult);
		
		return Future;
	}

	virtual TFuture<UBF::FLoadTextureResult> GetTextureResource(const FString& BlueprintId, const FString& ResourceId) override
	{
		TSharedPtr<TPromise<UBF::FLoadTextureResult>> Promise = MakeShareable(new TPromise<UBF::FLoadTextureResult>());
		TFuture<UBF::FLoadTextureResult> Future = Promise->GetFuture();
		UBF::FLoadTextureResult LoadResult;
		
		LoadResult.Result = TPair<bool, UTexture2D*>(false, nullptr);

		Promise->SetValue(LoadResult);
		
		return Future;
	}

	virtual TFuture<UBF::FLoadDataArrayResult> GetMeshResource(const FString& BlueprintId, const FString& ResourceId) override
	{
		TSharedPtr<TPromise<UBF::FLoadDataArrayResult>> Promise = MakeShareable(new TPromise<UBF::FLoadDataArrayResult>());
		TFuture<UBF::FLoadDataArrayResult> Future = Promise->GetFuture();
		UBF::FLoadDataArrayResult LoadResult;
		TArray<uint8> Data;
		LoadResult.Result = TPair<bool, TArray<uint8>>(false, Data);

		Promise->SetValue(LoadResult);
		
		return Future;
	}
	
private:
	TMap<FString, UBF::FGraphHandle> Graphs;
	TMap<FString, IBlueprintInstance*> Instances;
};
