// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "Graph.h"
#include "DataTypes/SceneNode.h"

class FUBFLogData;
class IGraphProvider;

namespace UBF
{
	/**
	 * Holds Unreal specific UserData for graph execution
	 */
	class FContextData
	{
	public:
		~FContextData()
		{
			PinnedWorld->Release();
		};

		FString BlueprintId;
		TSharedPtr<IExecutionSetConfig> ExecutionSetConfig;
		UGCPin* PinnedWorld;
		FGraphHandle Graph;
		TFunction<void(bool, FUBFExecutionReport)> OnGraphComplete;
		TFunction<void(FString, FFI::ScopeID)> OnNodeStart;
		TFunction<void(FString, FFI::ScopeID)> OnNodeComplete;
		
		explicit FContextData(const FString& BlueprintId, const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
			const FGraphHandle& Graph, TFunction<void(bool, FUBFExecutionReport)>&& OnGraphComplete,
			TFunction<void(FString, FFI::ScopeID)>&& OnNodeStart,
			TFunction<void(FString, FFI::ScopeID)>&& OnNodeComplete);

		void SetGraphReadyToComplete() const;
		void SetGraphComplete() const;

	private:
		void TryCompleteInternal() const;
		mutable bool bIsReadyToComplete = false;
		mutable bool bIsComplete = false;
	};
}

