#include "ExecutionSets/ExecutionSetHandle.h"

#include "ExecutionSets/ExecutionSetResult.h"

namespace UBF
{
	bool FExecutionSetHandle::IsComplete() const
	{
		if (SetResult.IsValid())
			return SetResult->IsComplete();

		return true;
	}

	bool FExecutionSetHandle::IsValid() const
	{
		return ExecutionSetConfig.IsValid() || SetResult.IsValid();
	}

	void FExecutionSetHandle::FlagShouldCancel() const
	{
		if (ExecutionSetConfig.IsValid())
			ExecutionSetConfig.Pin()->FlagCancelExecution();
	}
}