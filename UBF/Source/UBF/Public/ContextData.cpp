// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "ContextData.h"

#include "UBFLogData.h"
#include "ExecutionSets/IExecutionSetConfig.h"

namespace UBF
{
	FContextData::FContextData(const FString& BlueprintId, const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig, const FGraphHandle& Graph,
		TFunction<void(bool, FUBFExecutionReport)>&& OnComplete): BlueprintId(BlueprintId), Graph(Graph), OnComplete(MoveTemp(OnComplete))
	{
		if (ExecutionSetConfig->GetRoot()->GetWorld())
		{
			PinnedWorld = UGCPin::Pin(ExecutionSetConfig->GetRoot()->GetWorld());
		}
		else
		{
			ExecutionSetConfig->GetLogData()->Log(BlueprintId, EUBFLogLevel::Error, TEXT("Root GetWorld is Invalid"));
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
			FUBFExecutionReport ExecutionReport = ExecutionSetConfig->GetLogData()->CreateReport();

			if (UBFLogging::ShouldPrintLogSummary() && ExecutionSetConfig->GetLogData()->GetRootBlueprintId() == BlueprintId)
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
			
			OnComplete(ExecutionReport.bWasSuccessful && !bCancelExecution, ExecutionReport);
		}
	}
}
