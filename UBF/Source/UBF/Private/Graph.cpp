#include "Graph.h"

#include "Dynamic.h"
#include "GraphProvider.h"
#include "Registry.h"
#include "SubGraphResolver.h"
#include "UBF.h"
#include "UBFLog.h"
#include "UBFUtils.h"

namespace UBF
{
	void FGraphHandle::Execute(
		const FString& BlueprintId,
		USceneComponent* Root,
		IGraphProvider* GraphProvider, ISubGraphResolver* SubGraphResolver,
		const TMap<FString, FDynamicHandle>& Inputs, const FCustomNode* ParentNode,
		TFunction<void()>&& OnComplete, FExecutionContextHandle& Handle) const
	{
		UE_LOG(LogUBF, VeryVerbose, TEXT("FGraphHandle::Execute Creating UserData"));
		check(Root);
		SubGraphResolver == nullptr ? new FDefaultSubGraphResolver() : SubGraphResolver;
		const FContextData* ContextData = new FContextData(BlueprintId, Root, GraphProvider, SubGraphResolver, *this, MoveTemp(OnComplete));
		const FDynamicHandle DynamicUserData(FDynamicHandle::ForeignHandled(ContextData));

		UE_LOG(LogUBF, VeryVerbose, TEXT("FGraphHandle::Execute"));

		FDynamicHandle DynamicMap = FDynamicHandle::Dictionary();

		for (const auto& Tuple : Inputs)
		{
			if (FDynamicHandle::IsNull(Tuple.Value)) continue;
			
			if (!DynamicMap.TrySet(Tuple.Key, Tuple.Value))
				UE_LOG(LogUBF, Warning, TEXT("Failed to set input with key %s into DynamicMap"), *Tuple.Key);
		}

		FExecutionContextHandle TempHandle(CALL_RUST_FUNC(graph_execute)(
			RustPtr,
			DynamicMap.GetRustPtr(),
			ParentNode ? ParentNode->GetRustContext() : nullptr,
			ParentNode ? TCHAR_TO_UTF16(*ParentNode->GetNodeId()) : nullptr,
			ParentNode ? ParentNode->GetNodeId().Len() : 0,
			DynamicUserData.GetRustPtr(),
			&FGraphHandle::OnComplete
		));

		Handle = TempHandle;
		ContextData->SetReadyToComplete();
	}
	
	void FGraphHandle::OnComplete(FFI::Dynamic* RawUserData)
	{
		UE_LOG(LogUBF, VeryVerbose, TEXT("FGraphHandle::OnComplete"));
		const FDynamicHandle UserData(RawUserData);
		
		const FContextData* ContextUserData;
		
		if (UserData.TryInterpretAs<const FContextData>(ContextUserData))
		{
			ContextUserData->SetComplete();
		}
		else
		{
			UE_LOG(LogUBF, Error,
				TEXT("Failed to get user data from graph execution context. OnComplete callback will not be called."));
		}
	}

	bool FGraphHandle::Load(const UBF::FRegistryHandle& Registry, const FString& Json, FGraphHandle& Graph)
	{
		const uint16_t* RawJson = TCHAR_TO_UTF16(*Json);
		const int32_t RawJsonLen = TCString<uint16_t>::Strlen(RawJson); // omit '\0'
		FFI::GraphInstance* RustPtr = CALL_RUST_FUNC(graph_load)(Registry.GetRustPtr(), RawJson, RawJsonLen);
		if (RustPtr == nullptr)
		{
			UE_LOG(LogUBF, Error, TEXT("Failed to parse Graph JSON %s"), *Json);
			return false;
		}
		
		Graph = FGraphHandle(RustPtr);
		return true;
	}
	
	void FGraphHandle::GetResources(TArray<FString>& Resources) const
	{
		check(this);
		check(IsInGameThread());

		auto Context = new TArray<FString>();

		auto ResourcesIterator = [](intptr_t Context, const uint8_t* Id, int32_t IdLen) -> bool
		{
			auto Resources = reinterpret_cast<TArray<FString>*>(Context);
			
			FString IdString = UBFUtils::FromBytesToString(Id, IdLen);
			
			UE_LOG(LogUBF, VeryVerbose, TEXT("Resource Id: %s"), *IdString);
			
			Resources->Add(IdString);

			return true;
		};
	
		CALL_RUST_FUNC(graph_iter_resources)(RustPtr, reinterpret_cast<intptr_t>(Context), ResourcesIterator);
		
		Resources.Append(*Context);
		
		// Clean up the context
		delete Context;
	}

	void FGraphHandle::GetOutputs(TArray<FBindingInfo>& Outputs) const
	{
		check(this);
		check(IsInGameThread());

		auto Context = new TArray<FBindingInfo>();
		
		auto OutputsIterator = [](intptr_t Context, const uint8_t* Id, int32_t IdLen,
			const uint8_t* Type, int32_t TypeLen, FFI::Dynamic* DynamicPtr) -> bool
		{
			auto Outputs = reinterpret_cast<TArray<FBindingInfo>*>(Context);
			
			FString IdString = UBFUtils::FromBytesToString(Id, IdLen);
			FString TypeString = UBFUtils::FromBytesToString(Type, TypeLen);
			FDynamicHandle Dynamic = FDynamicHandle(DynamicPtr);
			UE_LOG(LogUBF, VeryVerbose, TEXT("Output Id: %s"), *IdString);
			UE_LOG(LogUBF, VeryVerbose, TEXT("Output Type: %s"), *TypeString);
			UE_LOG(LogUBF, VeryVerbose, TEXT("Output Value: %s"), *Dynamic.ToString());
			
			Outputs->Add(FBindingInfo(IdString, TypeString, Dynamic));

			return true;
		};
		
		CALL_RUST_FUNC(graph_iter_outputs)(RustPtr, reinterpret_cast<intptr_t>(Context), OutputsIterator);

		Outputs.Append(*Context);
		
		delete Context;
	}

	void FGraphHandle::GetInputs(TArray<FBindingInfo>& Inputs) const
	{
		check(this);
		check(IsInGameThread());

		auto Context = new TArray<FBindingInfo>();
		
		auto InputsIterator = [](intptr_t Context, const uint8_t* Id, int32_t IdLen,
			const uint8_t* Type, int32_t TypeLen, FFI::Dynamic* DynamicPtr) -> bool
		{
			auto Outputs = reinterpret_cast<TArray<FBindingInfo>*>(Context);
			
			FString IdString = UBFUtils::FromBytesToString(Id, IdLen);
			FString TypeString = UBFUtils::FromBytesToString(Type, TypeLen);
			FDynamicHandle Dynamic = FDynamicHandle(DynamicPtr);
			UE_LOG(LogUBF, VeryVerbose, TEXT("Input Id: %s"), *IdString);
			UE_LOG(LogUBF, VeryVerbose, TEXT("Input Type: %s"), *TypeString);
			
			Outputs->Add(FBindingInfo(IdString, TypeString, Dynamic));

			return true;
		};
		
		CALL_RUST_FUNC(graph_iter_inputs)(RustPtr, reinterpret_cast<intptr_t>(Context), InputsIterator);

		Inputs.Append(*Context);
		
		delete Context;
	}
}
