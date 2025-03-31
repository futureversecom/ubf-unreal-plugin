#include "ExecutionSets/ExecutionSetData.h"

#include "ExecutionSets/ExecutionSetConfig.h"

namespace UBF
{
	FExecutionSetData::FExecutionSetData(FSceneNode* InRootNode, const TArray<FExecutionInstanceData>& Datas,
		TFunction<void(bool, TSharedPtr<FExecutionSetResult>)>&& InOnComplete) : ExecutionInstanceDatas(Datas), RootNode(InRootNode), OnComplete(InOnComplete)
	{
		
	}

	TSharedPtr<IExecutionSetConfig> FExecutionSetData::CreateExecutionSetConfig() const
	{
		return MakeShared<FExecutionSetConfig>(ExecutionInstanceDatas, RootNode);
	}

	TFunction<void(bool, TSharedPtr<FExecutionSetResult>)> FExecutionSetData::GetOnComplete() const
	{
		return OnComplete;
	}
}
