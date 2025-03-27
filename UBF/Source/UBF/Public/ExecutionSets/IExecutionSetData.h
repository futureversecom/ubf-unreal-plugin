#pragma once
#include "ExecutionContext.h"

namespace UBF
{
	class IExecutionSetConfig;
	
	class IExecutionSetData
	{
	public:
		virtual ~IExecutionSetData() = default;
		
		virtual TSharedPtr<IExecutionSetConfig> CreateExecutionSetConfig() = 0;
		virtual void SetExecutionContext(const FExecutionContextHandle& ExecutionContext) = 0;
		virtual TFunction<void(bool, FUBFExecutionReport)> GetOnComplete() = 0;
	};
}


