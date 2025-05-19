#pragma once

#include "ExecutionInstanceData.h"
#include "IExecutionSetData.h"


namespace UBF
{
	class UBF_API FExecutionSetData : public IExecutionSetData
	{
	public:
		FExecutionSetData() {}
		FExecutionSetData(const TSharedPtr<FSceneNode>& InRootNode, const TArray<FExecutionInstanceData>& Datas, TFunction<void(bool, TSharedPtr<FExecutionSetResult>)>&& InOnComplete);
		FExecutionSetData(USceneComponent* InRootNode, const TArray<FExecutionInstanceData>& Datas, TFunction<void(bool, TSharedPtr<FExecutionSetResult>)>&& InOnComplete);

		virtual TSharedPtr<IExecutionSetConfig> CreateExecutionSetConfig() const override;
		virtual TFunction<void(bool, TSharedPtr<FExecutionSetResult>)> GetOnComplete() const override;
		
		TArray<FExecutionInstanceData> ExecutionInstanceDatas;
		TSharedPtr<FSceneNode> RootNode = nullptr;
		TFunction<void(bool, TSharedPtr<FExecutionSetResult>)> OnComplete;

		void AddInputsForId(const FString& InstanceID, const TMap<FString, FDynamicHandle>& Inputs);
		int GetIndexForId(const FString& InstanceID) const;
	};
}
