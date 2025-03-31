#pragma once
#include "IExecutionSetConfig.h"


namespace UBF
{
	class FExecutionSetResult;
}

struct FExecutionSetHandle
{
public:
	FExecutionSetHandle() {}
	FExecutionSetHandle(const TSharedPtr<UBF::IExecutionSetConfig>& SetConfig, const TSharedPtr<UBF::FExecutionSetResult>& SetResult) : ExecutionSetConfig(SetConfig.ToWeakPtr()), SetResult(SetResult) {}
	
	bool IsComplete() const;
	bool IsValid() const;

	void FlagShouldCancel() const;

	// TODO maybe get offer GetResult()?
	// TODO maybe keep SharedPtr for SetResult
private:
	TWeakPtr<UBF::IExecutionSetConfig> ExecutionSetConfig;
	TWeakPtr<UBF::FExecutionSetResult> SetResult;
};
