// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "Graph.h"

#include "Dynamic.h"
#include "Registry.h"
#include "UBF.h"
#include "UBFLog.h"
#include "UBFUtils.h"
#include "ExecutionSets/IExecutionSetConfig.h"

namespace UBF
{
	void FGraphHandle::Execute(
		const FString& BlueprintId,
		const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
		const TMap<FString, FDynamicHandle>& Inputs,
		TFunction<void(bool, FUBFExecutionReport)>&& OnComplete, FExecutionContextHandle& Handle) const
	{
		UE_LOG(LogUBF, Log, TEXT("Executing Graph Id: %s version: %s"), *BlueprintId, *GetGraphVersion().ToString());
		
		if (!ExecutionSetConfig->GetRoot().IsValid() || !IsValid(ExecutionSetConfig->GetRoot()->GetAttachmentComponent()))
		{
			UE_LOG(LogUBF, Verbose, TEXT("FGraphHandle::Execute Root is invalid, aborting execution"));
			return;
		}
		
		const FContextData* ContextData = new FContextData(BlueprintId, ExecutionSetConfig, *this, MoveTemp(OnComplete));
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
