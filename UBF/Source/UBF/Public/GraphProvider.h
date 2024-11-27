// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph.h"
#include "Registry.h"
#include "UObject/Interface.h"

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

	struct UBF_API FLoadTextureResult final : TLoadResult<UTexture*> {};

	struct UBF_API FLoadMeshResult final : TLoadResult<UglTFRuntimeAsset*> {};
	
	struct UBF_API FLoadStringResult final : TLoadResult<FString> {};
	
	struct UBF_API FLoadDataArrayResult final : TLoadResult<TArray<uint8>> {};
}

class UBF_API IGraphProvider
{
public:
	virtual TFuture<UBF::FLoadGraphResult> GetGraph(const FString& BlueprintId) = 0;
	
	virtual TFuture<UBF::FLoadTextureResult> GetTextureResource(const FString& BlueprintId, const FString& ResourceId) = 0;
	
	virtual TFuture<UBF::FLoadDataArrayResult> GetMeshResource(const FString& BlueprintId, const FString& ResourceId) = 0;

	virtual ~IGraphProvider() {}
};


class FDefaultGraphProvider final : public IGraphProvider
{
	
public:
	void RegisterGraph(const FString& Id, const UBF::FGraphHandle& Graph)
	{
		Graphs.Add(Id, Graph);
	}

	virtual TFuture<UBF::FLoadGraphResult> GetGraph(const FString& BlueprintId) override
	{
		TSharedPtr<TPromise<UBF::FLoadGraphResult>> Promise = MakeShareable(new TPromise<UBF::FLoadGraphResult>());
		TFuture<UBF::FLoadGraphResult> Future = Promise->GetFuture();
		UBF::FLoadGraphResult LoadResult;

		LoadResult.Result = Graphs.Contains(BlueprintId)
			? TPair<bool, UBF::FGraphHandle>(true, Graphs[BlueprintId])
			: TPair<bool, UBF::FGraphHandle>(false, UBF::FGraphHandle());
	
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
};
