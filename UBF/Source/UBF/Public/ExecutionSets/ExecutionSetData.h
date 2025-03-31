#pragma once

#include "ExecutionInstanceData.h"
#include "IExecutionSetData.h"


namespace UBF
{
	class FExecutionSetData : public IExecutionSetData
	{
	public:
		FExecutionSetData() {}
		FExecutionSetData(FSceneNode* InRootNode, const TArray<FExecutionInstanceData>& Datas, TFunction<void(bool, TSharedPtr<FExecutionSetResult>)>&& InOnComplete);

		virtual TSharedPtr<IExecutionSetConfig> CreateExecutionSetConfig() const override;
		virtual TFunction<void(bool, TSharedPtr<FExecutionSetResult>)> GetOnComplete() const override;
		
		TArray<FExecutionInstanceData> ExecutionInstanceDatas;
		FSceneNode* RootNode = nullptr;
		TFunction<void(bool, TSharedPtr<FExecutionSetResult>)> OnComplete;
	};
}
