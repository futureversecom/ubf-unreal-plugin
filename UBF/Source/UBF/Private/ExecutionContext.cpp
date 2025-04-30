// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "ExecutionContext.h"

#include "UBFLog.h"
#include "ExecutionSets/IExecutionSetConfig.h"
#include "UBFLogData.h"
#include "UBFUtils.h"

namespace UBF
{
	FSceneNode* FExecutionContextHandle::GetRoot() const
	{
		return ContextData->ExecutionSetConfig->GetRoot().Get();
	}

	UWorld* FExecutionContextHandle::GetWorld() const
	{
		check(this);
			
		if (ContextData == nullptr)
			DynamicContextData.TryInterpretAs(ContextData);
			
		check(ContextData);
		check(ContextData->ExecutionSetConfig.IsValid());
		check(ContextData->ExecutionSetConfig->GetRoot().IsValid());

		if (USceneComponent* SceneComponent = ContextData->ExecutionSetConfig->GetRoot()->GetAttachmentComponent())
		{
			if (!IsValid(SceneComponent))
				return nullptr;

			return SceneComponent->GetWorld();
		}
			
		return nullptr;
	}

	bool FExecutionContextHandle::GetCancelExecution() const
	{
		if (ContextData == nullptr)
			return false;

		return ContextData->ExecutionSetConfig->GetCancelExecution();
	}

	void FExecutionContextHandle::Log(EUBFLogLevel Level, const FString& Log) const
	{
		if (!GetUserData()) return;

		GetUserData()->ExecutionSetConfig->GetLogData()->Log(GetUserData()->BlueprintId, Level, Log);
	}

	void FExecutionContextHandle::PrintBlueprintDebug(const FString& ContextString) const
	{
		
	}

	bool FExecutionContextHandle::TryTriggerNode(FString const& SourceNodeId, FString const& SourcePortKey, FFI::ScopeID ScopeID) const
	{
		uint32_t ChildScope = 0;
		
		uint8_t Result = CALL_RUST_FUNC(ctx_trigger_node)(
			RustPtr,
			TCHAR_TO_UTF16(*SourceNodeId),
			SourceNodeId.Len(),
			TCHAR_TO_UTF16(*SourcePortKey),
			SourcePortKey.Len(),
			ScopeID,
			&ChildScope
		);

		if (Result == 1 /* pending */)
		{
			// TODO add to pending scopes?
		}

		if (Result == 2 /* error */)
		{
			return false;
		}

		return true;
	}

	bool FExecutionContextHandle::TryReadInput(FString const& NodeId, const FString& PortKey, FFI::ScopeID ScopeID,
	                                           FDynamicHandle& Dynamic) const
	{
		FFI::Dynamic* DynamicPtr;
		if (!CALL_RUST_FUNC(ctx_read_input)(
			RustPtr,
			TCHAR_TO_UTF16(*NodeId),
			NodeId.Len(),
			TCHAR_TO_UTF16(*PortKey),
			PortKey.Len(),
			ScopeID,
			&DynamicPtr))
		{
			UBF_LOG(Warning, TEXT("No Input Found (Node:%s Port:%s) on BlueprintId: %s"), *NodeId, *FString(PortKey), *GetBlueprintID());
			PrintBlueprintDebug(FString::Printf(TEXT("No Input Found (Node:%s Port:%s)"), *NodeId, *FString(PortKey)));
			return false;
		}
		Dynamic = FDynamicHandle(DynamicPtr);
		return true;
	}

	void FExecutionContextHandle::WriteOutput(const FString& NodeId, const FString& PortKey, const FDynamicHandle& Dynamic) const
	{
		CALL_RUST_FUNC(ctx_write_output)(
			RustPtr,
			TCHAR_TO_UTF16(*NodeId),
			NodeId.Len(),
			TCHAR_TO_UTF16(*PortKey),
			PortKey.Len(),
			Dynamic.GetRustPtr()
		);
	}

	bool FExecutionContextHandle::GetDynamicDataEntry(const FString& Key, FDynamicHandle& DynamicHandle) const
	{
		FFI::Dynamic* DynamicPtr;
		bool bResult = CALL_RUST_FUNC(ctx_get_dynamic_data_entry)(
			RustPtr,
			TCHAR_TO_UTF16(*Key),
			Key.Len(),
			&DynamicPtr
		);
		DynamicHandle = FDynamicHandle(DynamicPtr);
		return bResult;
	}

	bool FExecutionContextHandle::SetDynamicDataEntry(const FString& Key, const FDynamicHandle& DynamicHandle) const
	{
		bool bResult = CALL_RUST_FUNC(ctx_set_dynamic_data_entry)(
			RustPtr,
			TCHAR_TO_UTF16(*Key),
			Key.Len(),
			DynamicHandle.GetRustPtr()
		);
		return bResult;
	}

	bool FExecutionContextHandle::GetDeclaredNodeInputs(const FString& NodeId, TArray<FString>& OutInputs) const
	{
		auto Context = new TArray<FString>();

		auto InputsIter = [](intptr_t Context, const uint8_t* Id, int32_t IdLen) -> bool
		{
			auto Inputs = reinterpret_cast<TArray<FString>*>(Context);
			
			FString Key = UBFUtils::FromBytesToString(Id, IdLen);
			
			Inputs->Add(Key);
			
			return true;
		};
		
		bool bResult = CALL_RUST_FUNC(ctx_get_declared_node_inputs)(
			RustPtr,
			TCHAR_TO_UTF16(*NodeId),
			NodeId.Len(),
			reinterpret_cast<intptr_t>(Context),
			InputsIter);

		OutInputs.Append(*Context);
		delete Context;
		return bResult;
	}

	bool FExecutionContextHandle::TryReadOutput(const FString& BindingId, FDynamicHandle& Dynamic) const
	{
		FFI::Dynamic* DynamicPtr;
		if (!RustPtr)
		{
			UBF_LOG(Warning, TEXT("FExecutionContextHandle::TryReadOutput RustPtr is invalid %s"), *BindingId);
			return false;
		}
		if (!CALL_RUST_FUNC(ctx_read_output)(
			RustPtr,
			TCHAR_TO_UTF16(*BindingId),
			BindingId.Len(),
			&DynamicPtr))
		{
			UBF_LOG(Warning, TEXT("No Output Found (%s)"), *BindingId);
			return false;
		}
		Dynamic = FDynamicHandle(DynamicPtr);
		return true;
	}
}
