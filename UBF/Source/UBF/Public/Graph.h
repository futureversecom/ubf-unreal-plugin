#pragma once

#include "CoreMinimal.h"
#include "Dynamic.h"
#include "Managed.h"

class ISubGraphResolver;
class IGraphProvider;

namespace UBF
{
	class FCustomNode;
	class FOnComplete;

	struct FRegistryHandle;
	struct FExecutionContextHandle;
	
	struct FBindingInfo
	{
		FString Id;
		FString Type;
		FDynamicHandle DynamicPtr = FDynamicHandle::Null();

		FBindingInfo() {}
		FBindingInfo(const FString& NewId, const FString& NewType, const FDynamicHandle& Dynamic)
		{
			Id = NewId;
			Type = NewType;
			DynamicPtr = Dynamic;
		};
	};

	struct FResourceInfo
	{
		FString Id;
		FString Uri;
		
		FResourceInfo() {}
		FResourceInfo(const FString& NewId, const FString& NewUri)
		{
			Id = NewId;
			Uri = NewUri;
		}

		FString ToString() const
		{
			return FString::Printf(TEXT("Id: %s Uri: %s"), *Id, *Uri);
		}
	};
	
	struct UBF_API FGraphHandle
	{
		IMPLEMENT_MANAGED_TYPE_CONSTRUCTOR(FGraphHandle, FFI::GraphInstance)
		IMPLEMENT_MANAGED_TYPE(FGraphHandle, FFI::GraphInstance, graph_release, graph_retain)
	public:
		
		void Execute(
			const FString& BlueprintId,
			USceneComponent* Root,
			IGraphProvider* GraphProvider,
			ISubGraphResolver* SubGraphResolver,
			const TMap<FString, FDynamicHandle>& Inputs,
			const FCustomNode* ParentNode,
			TFunction<void()>&& OnComplete,
			FExecutionContextHandle& Handle) const;
		
		static void OnComplete(FFI::Dynamic*);
		
		static bool Load(const FRegistryHandle& Registry, const FString &Json, FGraphHandle& Graph);

		void GetResources(TArray<FString>& Resources) const;
		
		void GetOutputs(TArray<FBindingInfo>& Outputs) const;
		void GetInputs(TArray<FBindingInfo>& Inputs) const;

	private:
	
		mutable FString CachedId;
	};
}
