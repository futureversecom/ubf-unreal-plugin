#pragma once
#include "Graph.h"
#include "IExecutionSetConfig.h"

namespace UBF
{
	class FExecutionInstance
	{
	public:
		void Execute(const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
			TFunction<void(bool, FUBFExecutionReport)>&& OnComplete,
			FExecutionContextHandle& Handle) const;
		
	protected:
		FString BlueprintId;
		FGraphHandle GraphHandle;
		TMap<FString, FDynamicHandle> Inputs;
	};

}
