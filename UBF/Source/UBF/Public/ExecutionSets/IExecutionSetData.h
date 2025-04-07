#pragma once
#include "ExecutionContext.h"


namespace UBF
{
	class IExecutionSetConfig;
	class FExecutionSetResult;
	
	class UBF_API IExecutionSetData
	{
	public:
		virtual ~IExecutionSetData() = default;
		
		virtual TSharedPtr<IExecutionSetConfig> CreateExecutionSetConfig() const = 0;
		virtual TFunction<void(bool, TSharedPtr<FExecutionSetResult>)> GetOnComplete() const = 0;
	};
}


