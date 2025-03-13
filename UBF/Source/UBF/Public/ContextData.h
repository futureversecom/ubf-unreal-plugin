#pragma once

#include "BlueprintInstance.h"
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
			delete Root;
		};

		FString BlueprintId;
		FSceneNode* Root;
		TSharedPtr<IGraphProvider> GraphProvider;
		TSharedPtr<FUBFLogData> LogData;
		TMap<FString, FBlueprintInstance> InstancedBlueprints;
		UGCPin* PinnedWorld;
		FGraphHandle Graph;
		bool bCancelExecution = false;
		TFunction<void(bool, FUBFExecutionReport)> OnComplete;
		
		explicit FContextData(const FString& BlueprintId, USceneComponent* Root, const TSharedPtr<IGraphProvider>& GraphProvider, const TSharedPtr<FUBFLogData>& LogData, const TMap<FString, FBlueprintInstance>& InstancedBlueprints,
			const FGraphHandle& Graph, TFunction<void(bool, FUBFExecutionReport)>&& OnComplete);

		void SetReadyToComplete() const;
		void SetComplete() const;

	private:
		void TryCompleteInternal() const;
		mutable bool bIsReadyToComplete = false;
		mutable bool bIsComplete = false;
	};
}

