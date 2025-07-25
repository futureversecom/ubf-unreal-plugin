// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "Graph.h"

#include "Dynamic.h"
#include "Registry.h"
#include "UBF.h"
#include "UBFLog.h"
#include "Util/UBFUtils.h"
#include "ExecutionSets/IExecutionSetConfig.h"

namespace UBF
{
	void FGraphHandle::Execute(
		const FString& BlueprintId,
		const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
		const TMap<FString, FDynamicHandle>& Inputs,
		TFunction<void(bool, FUBFExecutionReport)>&& OnComplete,
		TFunction<void(FString, FFI::ScopeID)>&& OnNodeStart,
		TFunction<void(FString, FFI::ScopeID)>&& OnNodeComplete,
		FExecutionContextHandle& Handle) const
	{
		UE_LOG(LogUBF, Log, TEXT("Executing Graph Id: %s version: %s"), *BlueprintId, *GetGraphVersion().ToString());
		
		if (!ExecutionSetConfig->GetRoot().IsValid() || !IsValid(ExecutionSetConfig->GetRoot()->GetAttachmentComponent()))
		{
			UE_LOG(LogUBF, Verbose, TEXT("FGraphHandle::Execute Root is invalid, aborting execution"));
			OnComplete(false, FUBFExecutionReport::Failure());
			return;
		}
		
		const FContextData* ContextData = new FContextData(BlueprintId, ExecutionSetConfig, *this,
			MoveTemp(OnComplete), MoveTemp(OnNodeStart), MoveTemp(OnNodeComplete));
		
		const FDynamicHandle DynamicUserData(FDynamicHandle::ForeignHandled(ContextData));
		
		FDynamicHandle DynamicMap = FDynamicHandle::Dictionary();

		for (const auto& Tuple : Inputs)
		{
			if (FDynamicHandle::IsNull(Tuple.Value)) continue;
			
			if (!DynamicMap.TrySet(Tuple.Key, Tuple.Value))
				UE_LOG(LogUBF, Warning, TEXT("Failed to set input with key %s into DynamicMap"), *Tuple.Key);
		}
		
		if (!(GetGraphVersion() >= MinSupportedGraphVersion && GetGraphVersion() <= MaxSupportedGraphVersion))
		{
			UE_LOG(LogUBF, Warning, TEXT("Attemping to execute an unsupported Graph Version! It could produce unexpected result!"
			" Current Graph Version: %s MinSupportedVersion: %s Max Supported Graph Version: %s"),
			*GetGraphVersion().ToString(), *MinSupportedGraphVersion.ToString(), *MaxSupportedGraphVersion.ToString());
		}

		FExecutionContextHandle TempHandle(CALL_RUST_FUNC(graph_execute)(
			RustPtr,
			DynamicMap.GetRustPtr(),
			DynamicUserData.GetRustPtr(),
			TCHAR_TO_UTF16(*BlueprintId),
			BlueprintId.Len(),
			&FGraphHandle::OnGraphComplete,
			&FGraphHandle::OnNodeComplete,
			&FGraphHandle::OnNodeStart,
			&FGraphHandle::OnExecuteGraphLog
		));

		Handle = TempHandle;
		ContextData->SetGraphReadyToComplete();
	}
	
	void FGraphHandle::OnGraphComplete(FFI::Dynamic* RawUserData)
	{
		UE_LOG(LogUBF, VeryVerbose, TEXT("FGraphHandle::OnGrpahComplete"));
		const FDynamicHandle UserData(RawUserData);
		
		const FContextData* ContextUserData;
		
		if (UserData.TryInterpretAs<const FContextData>(ContextUserData))
		{
			ContextUserData->SetGraphComplete();
		}
		else
		{
			UE_LOG(LogUBF, Error,
				TEXT("Failed to get user data from graph execution context. OnComplete callback will not be called."));
		}
	}

	void FGraphHandle::OnNodeComplete(const uint8_t* NodeIdPtr, int32_t NodeIdLen, FFI::ScopeID ScopeID, FFI::Dynamic* RawUserData)
	{
		UE_LOG(LogUBF, VeryVerbose, TEXT("FGraphHandle::OnNodeComplete"));
		const FDynamicHandle UserData(RawUserData);
		
		const FContextData* ContextUserData;
		
		if (UserData.TryInterpretAs<const FContextData>(ContextUserData))
		{
			const FString NodeId = UBFUtils::FromBytesToString(NodeIdPtr, NodeIdLen); 
			ContextUserData->OnNodeComplete(NodeId, ScopeID);
		}
		else
		{
			UE_LOG(LogUBF, Error,
				TEXT("Failed to get user data from graph execution context. OnNodeComplete callback will not be called."));
		}
	}

	void FGraphHandle::OnExecuteGraphLog(const int32_t UBFLogLevel, const uint8_t* Message, int32_t MessageLength, FFI::Dynamic* Context)
	{
		// todo: make use of context
		OnGraphLoadLog(UBFLogLevel, Message, MessageLength);
	}

	void FGraphHandle::OnNodeStart(const uint8_t* NodeIdPtr, int32_t NodeIdLen, FFI::ScopeID ScopeID, FFI::Dynamic* RawUserData)
	{
		UE_LOG(LogUBF, VeryVerbose, TEXT("FGraphHandle::OnNodeStart"));
		const FDynamicHandle UserData(RawUserData);
		
		const FContextData* ContextUserData;
		
		if (UserData.TryInterpretAs<const FContextData>(ContextUserData))
		{
			const FString NodeId = UBFUtils::FromBytesToString(NodeIdPtr, NodeIdLen); 
			ContextUserData->OnNodeStart(NodeId, ScopeID);
		}
		else
		{
			UE_LOG(LogUBF, Error,
				TEXT("Failed to get user data from graph execution context. OnNodeStart callback will not be called."));
		}
	}

	bool FGraphHandle::Load(const UBF::FRegistryHandle& Registry, const FString& Json, FGraphHandle& Graph)
	{
		const uint16_t* RawJson = TCHAR_TO_UTF16(*Json);
		const int32_t RawJsonLen = TCString<uint16_t>::Strlen(RawJson); // omit '\0'
		FFI::GraphInstance* RustPtr = CALL_RUST_FUNC(graph_load)
		(Registry.GetRustPtr(), RawJson, RawJsonLen, &FGraphHandle::OnGraphLoadLog);
		if (RustPtr == nullptr)
		{
			UE_LOG(LogUBF, Error, TEXT("Failed to parse Graph JSON %s"), *Json);
			return false;
		}
		
		Graph = FGraphHandle(RustPtr);
		return true;
	}

	void FGraphHandle::OnGraphLoadLog(const int32_t UBFLogLevel, const uint8_t* Message, int32_t MessageLength)
	{
		FString MessageString = UBFUtils::FromBytesToString(Message, MessageLength);
		switch (UBFLogLevel)
		{
			case 1:
				UE_LOG(LogUBF, Warning, TEXT("[NativeLog] %s"), *MessageString);
				break;
			case 2:
				UE_LOG(LogUBF, Error, TEXT("[NativeLog] %s"), *MessageString);
				break;
			default:
				UE_LOG(LogUBF, VeryVerbose, TEXT("[NativeLog] %s"), *MessageString);
				break;
		}
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

	FGraphVersion FGraphHandle::GetGraphVersion() const
	{
		if (CachedVersion.IsValid()) return CachedVersion;
		
		FString OutString;
		uintptr_t OutStringLen;
		const uint16_t* OutStringPtr = TCHAR_TO_UTF16(*OutString);
		CALL_RUST_FUNC(graph_version)(RustPtr, &OutStringPtr, &OutStringLen);
		const FString VersionString = UBFUtils::FromBytesToString(OutStringPtr, OutStringLen);

		CachedVersion = FGraphVersion(VersionString);
		return CachedVersion;
	}
}
