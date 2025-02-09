#include "ExecutionContext.h"

#include "UBFLog.h"
#include "GraphProvider.h"
#include "UBFUtils.h"

namespace UBF
{
	void FContextData::SetReadyToComplete() const
	{
		bIsReadyToComplete = true;

		if (bIsReadyToComplete && bIsComplete)
			OnComplete();
	}

	void FContextData::SetComplete() const
	{
		bIsComplete = true;

		if (bIsReadyToComplete && bIsComplete)
			OnComplete();
	}

	void FExecutionContextHandle::PrintBlueprintDebug(const FString& ContextString) const
	{
		if (ContextData == nullptr || ContextData->GraphProvider == nullptr)
			return;

		ContextData->GraphProvider->PrintBlueprintDebug(GetBlueprintID(), ContextString);
	}

	bool FExecutionContextHandle::TryTriggerNode(FString const& SourceNodeId, FString const& SourcePortKey) const
	{
		return CALL_RUST_FUNC(ctx_trigger_node)(
			RustPtr,
			TCHAR_TO_UTF16(*SourceNodeId),
			SourceNodeId.Len(),
			TCHAR_TO_UTF16(*SourcePortKey),
			SourcePortKey.Len()
		);
	}

	bool FExecutionContextHandle::						TryReadInput(FString const& NodeId, const FString& PortKey,
	                                           FDynamicHandle& Dynamic) const
	{
		FFI::Dynamic* DynamicPtr;
		if (!CALL_RUST_FUNC(ctx_read_input)(
			RustPtr,
			TCHAR_TO_UTF16(*NodeId),
			NodeId.Len(),
			TCHAR_TO_UTF16(*PortKey),
			PortKey.Len(),
			&DynamicPtr))
		{
			UE_LOG(LogUBF, Warning, TEXT("No Input Found (Node:%s Port:%s) on BlueprintId: %s"), *NodeId, *FString(PortKey), *GetBlueprintID());
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
			UE_LOG(LogUBF, Warning, TEXT("FExecutionContextHandle::TryReadOutput RustPtr is invalid %s"), *BindingId);
			return false;
		}
		if (!CALL_RUST_FUNC(ctx_read_output)(
			RustPtr,
			TCHAR_TO_UTF16(*BindingId),
			BindingId.Len(),
			&DynamicPtr))
		{
			UE_LOG(LogUBF, Warning, TEXT("No Output Found (%s)"), *BindingId);
			return false;
		}
		Dynamic = FDynamicHandle(DynamicPtr);
		return true;
	}
}
