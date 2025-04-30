// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "ContextData.h"

#include "UBFLogData.h"
#include "ExecutionSets/IExecutionSetConfig.h"

namespace UBF
{
	FContextData::FContextData(const FString& BlueprintId, const TSharedPtr<IExecutionSetConfig>& ExecutionSetConfig,
		const FGraphHandle& Graph, TFunction<void(bool, FUBFExecutionReport)>&& OnGraphComplete,
		TFunction<void(FString, FFI::ScopeID)>&& OnNodeStart, TFunction<void(FString, FFI::ScopeID)>&& OnNodeComplete)
		: BlueprintId(BlueprintId), ExecutionSetConfig(ExecutionSetConfig), Graph(Graph),
		OnGraphComplete(MoveTemp(OnGraphComplete)), OnNodeStart(MoveTemp(OnNodeStart)), OnNodeComplete(MoveTemp(OnNodeComplete))
	{
		if (ExecutionSetConfig->GetRoot().IsValid() && IsValid(ExecutionSetConfig->GetRoot()->GetWorld()))
		{
			PinnedWorld = UGCPin::Pin(ExecutionSetConfig->GetRoot()->GetWorld());
		}
		else
		{
			ExecutionSetConfig->GetLogData()->Log(BlueprintId, EUBFLogLevel::Error, TEXT("Root GetWorld is Invalid"));
		}
	}

	void FContextData::SetGraphReadyToComplete() const
	{
		bIsReadyToComplete = true;

		TryCompleteInternal();
	}

	void FContextData::SetGraphComplete() const
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
			
			OnGraphComplete(ExecutionReport.bWasSuccessful && !ExecutionSetConfig->GetCancelExecution(), ExecutionReport);
		}
	}
}
