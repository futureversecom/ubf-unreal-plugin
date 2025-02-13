#include "ContextData.h"

#include "UBFLogData.h"

namespace UBF
{
	FContextData::FContextData(const FString& BlueprintId, USceneComponent* Root,
		const TSharedPtr<IGraphProvider>& GraphProvider, const TSharedPtr<FUBFLogData>& LogData,
		const TMap<FString, FBlueprintInstance>& InstancedBlueprints, const FGraphHandle& Graph,
		TFunction<void(bool, FUBFExecutionReport)>&& OnComplete): BlueprintId(BlueprintId), Root(new FSceneNode(Root)), GraphProvider(GraphProvider), LogData(LogData), InstancedBlueprints(InstancedBlueprints) ,Graph(Graph), OnComplete(MoveTemp(OnComplete))
	{
		if (Root->GetWorld())
		{
			PinnedWorld = UGCPin::Pin(Root->GetWorld());
		}
		else
		{
			LogData->Log(BlueprintId, EUBFLogLevel::Error, TEXT("Root GetWorld is Invalid"));
		}
	}

	void FContextData::SetReadyToComplete() const
	{
		bIsReadyToComplete = true;

		TryCompleteInternal();
	}

	void FContextData::SetComplete() const
	{
		bIsComplete = true;

		TryCompleteInternal();
	}

	void FContextData::TryCompleteInternal() const
	{
		if (bIsReadyToComplete && bIsComplete)
		{
			FUBFExecutionReport ExecutionReport = LogData->CreateReport();

			if (UBFLogging::ShouldPrintLogSummary() && LogData->GetRootBlueprintId() == BlueprintId)
			{
				if (ExecutionReport.bWasSuccessful)
				{
					UE_LOG(LogUBF, Log, TEXT("%s"), *ExecutionReport.Summary);
				}
				else
				{
					UE_LOG(LogUBF, Error, TEXT("%s"), *ExecutionReport.Summary);
				}
			}
			
			OnComplete(ExecutionReport.bWasSuccessful, ExecutionReport);
		}
	}
}
