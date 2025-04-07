#include "ExecutionSets/ExecutionInstance.h"

#include "ExecutionSets/ExecutionSetResult.h"

UBF::FExecutionInstance::FExecutionInstance(const FString& BlueprintId, const FGraphHandle& GraphHandle) : BlueprintId(BlueprintId), GraphHandle(GraphHandle)
{
	
}

void UBF::FExecutionInstance::SetInputs(const TMap<FString, FDynamicHandle>& InInputs)
{
	Inputs = InInputs;
}

void UBF::FExecutionInstance::Execute(const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
                                      TFunction<void(bool, FUBFExecutionReport)>&& OnComplete, FExecutionContextHandle& Handle) const
{
	GraphHandle.Execute(BlueprintId, ExecutionSetConfig, Inputs, MoveTemp(OnComplete), Handle);
}

void UBF::FExecutionInstance::ExecuteWithInputs(const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
	TFunction<void(bool, FUBFExecutionReport)>&& OnComplete, const TMap<FString, FDynamicHandle>& NewInputs,
	FExecutionContextHandle& Handle) const
{
	TMap<FString, FDynamicHandle> CombinedInputs = Inputs;
	CombinedInputs.Append(NewInputs);
	GraphHandle.Execute(BlueprintId, ExecutionSetConfig, CombinedInputs, MoveTemp(OnComplete), Handle);
}
