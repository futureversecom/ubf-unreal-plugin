#include "ExecutionSets/ExecutionSetHandle.h"

#include "ExecutionSets/ExecutionSetResult.h"

bool FExecutionSetHandle::IsComplete() const
{
	if (SetResult.IsValid())
		return SetResult.Pin()->IsComplete();

	return true;
}

bool FExecutionSetHandle::IsValid() const
{
	return ExecutionSetConfig.IsValid();
}

void FExecutionSetHandle::FlagShouldCancel() const
{
	if (ExecutionSetConfig.IsValid())
		ExecutionSetConfig.Pin()->FlagCancelExecution();
}
