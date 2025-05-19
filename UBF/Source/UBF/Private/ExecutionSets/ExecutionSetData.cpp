#include "ExecutionSets/ExecutionSetData.h"

#include "ExecutionSets/ExecutionSetConfig.h"

namespace UBF
{
	FExecutionSetData::FExecutionSetData(const TSharedPtr<FSceneNode>& InRootNode, const TArray<FExecutionInstanceData>& Datas,
	                                     TFunction<void(bool, TSharedPtr<FExecutionSetResult>)>&& InOnComplete) : ExecutionInstanceDatas(Datas), RootNode(InRootNode), OnComplete(InOnComplete)
	{
		
	}

	FExecutionSetData::FExecutionSetData(USceneComponent* InRootNode, const TArray<FExecutionInstanceData>& Datas,
		TFunction<void(bool, TSharedPtr<FExecutionSetResult>)>&& InOnComplete) : ExecutionInstanceDatas(Datas), RootNode(MakeShared<FSceneNode>(InRootNode)), OnComplete(InOnComplete)
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

	void FExecutionSetData::AddInputsForId(const FString& InstanceID, const TMap<FString, FDynamicHandle>& Inputs)
	{
		int TargetIndex = GetIndexForId(InstanceID);

		if (!ExecutionInstanceDatas.IsValidIndex(TargetIndex))
			TargetIndex = ExecutionInstanceDatas.Add(FExecutionInstanceData(InstanceID));

		ExecutionInstanceDatas[TargetIndex].AddInputs(Inputs);
	}

	int FExecutionSetData::GetIndexForId(const FString& InstanceID) const
	{
		for (int i = 0; i < ExecutionInstanceDatas.Num(); i++)
		{
			if (ExecutionInstanceDatas[i].GetInstanceId() == InstanceID)
				return i;
		}

		return -1;
	}
}
