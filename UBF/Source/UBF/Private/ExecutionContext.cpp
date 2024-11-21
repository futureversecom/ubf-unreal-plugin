#include "ExecutionContext.h"

#include "UBFLog.h"
#include "UBFUtils.h"

namespace UBF
{
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
	
	template <class T>
	bool FExecutionContextHandle::TryReadInput(FString const& NodeId, const FString& PortKey, T*& Out) const
	{
		FDynamicHandle Ptr;
		if (!TryReadInput(NodeId, PortKey, Ptr))
		{
			UE_LOG(LogUBF, Warning, TEXT("Failed to read input (Node:%s Port:%s)"), *NodeId, *FString(PortKey));
			return false;
		}
		
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			UObject* Input;
			if (!Ptr.TryInterpretAs(Input))
			{
				UE_LOG(LogUBF, Warning, TEXT("Failed to read input as UObject (%s->%s)"), *NodeId, *FString(PortKey));
				return false;
			}
			Out = Cast<T>(Input);
			return true;
		}
		else
		{
			return Ptr.TryInterpretAs<T>(Out);
		}
	}

	template <typename T>
	bool FExecutionContextHandle::TryReadInputValue(FString const& NodeId, const FString& PortKey, T& Out) const
	{
		FDynamicHandle Ptr;
		if (!TryReadInput(NodeId, PortKey, Ptr))
		{
			UE_LOG(LogUBF, Warning, TEXT("Failed to read input (%s->%s)"), *NodeId, *FString(PortKey));
			return false;
		}

		UE_LOG(LogUBF, VeryVerbose, TEXT("Read dynamic input value %s"), *Ptr.ToString());

		return Ptr.TryInterpretAs(Out);
	}

	template <class T>
	bool FExecutionContextHandle::TryReadInputArray(FString const& NodeId, const FString& PortKey, TArray<T*>& Out) const
	{
		FDynamicHandle DynamicArray;
		if (!TryReadInput(NodeId, PortKey, DynamicArray))
		{
			UE_LOG(LogUBF, Warning, TEXT("Failed to read input (Node:%s Port:%s)"), *NodeId, *FString(PortKey));
			return false;
		}

		return DynamicArray.TryInterpretAsArray<T*>(Out);
	}

	template <class T>
	bool FExecutionContextHandle::TryReadInputValueArray(FString const& NodeId, const FString& PortKey,
		TArray<T>& Out) const
	{
		FDynamicHandle DynamicArray;
		if (!TryReadInput(NodeId, PortKey, DynamicArray))
		{
			UE_LOG(LogUBF, Warning, TEXT("Failed to read input (Node:%s Port:%s)"), *NodeId, *FString(PortKey));
			return false;
		}

		return DynamicArray.TryInterpretAsArray<T>(Out);
	}

	bool FExecutionContextHandle::TryReadInput(FString const& NodeId, const FString& PortKey,
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
			UE_LOG(LogUBF, Warning, TEXT("No Input Found (Node:%s Port:%s)"), *NodeId, *FString(PortKey));
			return false;
		}
		Dynamic = FDynamicHandle(DynamicPtr);
		return true;
	}

	void FExecutionContextHandle::TryGetResourcesWithFilter(const FString& Filter, TArray<FString>& FilteredResources) const
	{
		// TODO: reimplement this after refactor
		// TArray<FString> GraphResources;
		// UserData->Graph.GetResources(GraphResources);
		//
		// for (auto& ResourceInfo : GraphResources)
		// {
		// 	
		// 	if (!ResourceInfo.Uri.ToLower().Contains(Filter)) continue;
		//
		// 	FilteredResources.Add(ResourceInfo);
		// }
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

	bool FExecutionContextHandle::GetCurrentNodeInputs(const FString& NodeID, TMap<FString, FDynamicHandle>& InputMap) const
	{
		auto Context = new TMap<FString, FDynamicHandle>();
		
		auto InputsIter = [](intptr_t Context, const uint8_t* Id, int32_t IdLen, FFI::Dynamic* Dynamic) -> bool
		{
			auto Inputs = reinterpret_cast<TMap<FString, FDynamicHandle>*>(Context);
			
			FString Key = UBFUtils::FromBytesToString(Id, IdLen);
			
			FDynamicHandle DynamicHandle(Dynamic);

			Inputs->Add(Key, DynamicHandle);
			
			return true;
		};
		
		bool bResult = CALL_RUST_FUNC(ctx_get_all_node_inputs)(
			RustPtr,
			reinterpret_cast<intptr_t>(Context),
			TCHAR_TO_UTF16(*NodeID),
			NodeID.Len(),
			InputsIter
		);
		
		InputMap.Append(*Context);
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
