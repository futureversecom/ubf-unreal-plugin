#include "ExecutionSets/ExecutionInstance.h"

void UBF::FExecutionInstance::Execute(const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig, TFunction<void(bool, FUBFExecutionReport)>&& OnComplete,
	FExecutionContextHandle& Handle) const
{
	GraphHandle.Execute(BlueprintId, ExecutionSetConfig, Inputs, MoveTemp(OnComplete), Handle);
}
