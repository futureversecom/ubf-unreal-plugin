#pragma once

#include "Graph.h"

namespace UBF
{
	class UBF_API FExecutionInstance
	{
	public:
		FExecutionInstance(const FString& BlueprintId, const FGraphHandle& GraphHandle);

		void SetInputs(const TMap<FString, FDynamicHandle>& InInputs);
		
		void Execute(const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
			TFunction<void(bool, FUBFExecutionReport)>&& OnGraphComplete,
			TFunction<void(FString, FFI::ScopeID)>&& OnNodeStart,
			TFunction<void(FString, FFI::ScopeID)>&& OnNodeComplete,
			FExecutionContextHandle& Handle) const;

		void ExecuteWithInputs(const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
			TFunction<void(bool, FUBFExecutionReport)>&& OnGraphComplete,
			TFunction<void(FString, FFI::ScopeID)>&& OnNodeStart,
            TFunction<void(FString, FFI::ScopeID)>&& OnNodeComplete,
            const TMap<FString, FDynamicHandle>& NewInputs,
			FExecutionContextHandle& Handle) const;

		const FGraphHandle& GetGraphHandleRef() const {return GraphHandle;}
	protected:
		FString BlueprintId;
		FGraphHandle GraphHandle;
		TMap<FString, FDynamicHandle> Inputs;
	};

}
