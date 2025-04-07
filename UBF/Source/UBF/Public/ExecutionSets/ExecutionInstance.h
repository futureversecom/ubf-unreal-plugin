#pragma once
#include "Graph.h"
#include "IExecutionSetConfig.h"

namespace UBF
{
	class UBF_API FExecutionInstance
	{
	public:
		FExecutionInstance(const FString& BlueprintId, const FGraphHandle& GraphHandle);
		
		void Execute(const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
			TFunction<void(bool, FUBFExecutionReport)>&& OnComplete,
			FExecutionContextHandle& Handle) const;

		void ExecuteWithInputs(const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
			TFunction<void(bool, FUBFExecutionReport)>&& OnComplete, const TMap<FString, FDynamicHandle>& NewInputs,
			FExecutionContextHandle& Handle) const;

		const FGraphHandle& GetGraphHandleRef() const {return GraphHandle;}
	protected:
		FString BlueprintId;
		FGraphHandle GraphHandle;
		TMap<FString, FDynamicHandle> Inputs;
	};

}
