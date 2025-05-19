#pragma once
#include "IExecutionSetConfig.h"


namespace UBF
{
	class FExecutionSetResult;

	
	struct UBF_API FExecutionSetHandle
	{
	public:
		FExecutionSetHandle() {}
		FExecutionSetHandle(const TSharedPtr<UBF::IExecutionSetConfig>& SetConfig, const TSharedPtr<UBF::FExecutionSetResult>& SetResult) : ExecutionSetConfig(SetConfig.ToWeakPtr()), SetResult(SetResult) {}
	
		bool IsComplete() const;
		bool IsValid() const;

		void FlagShouldCancel() const;

		TSharedPtr<FExecutionSetResult> GetResult() const {return SetResult;}
	private:
		TWeakPtr<IExecutionSetConfig> ExecutionSetConfig;
		TSharedPtr<FExecutionSetResult> SetResult;
	};
}

