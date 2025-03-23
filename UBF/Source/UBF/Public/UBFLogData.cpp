// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "UBFLogData.h"

#include "UBFLog.h"


void FUBFLogData::Log(const FString& BlueprintId, EUBFLogLevel Level, const FString& Log)
{
	FLogData LogData;
	LogData.Level = Level;
	LogData.Log = Log;
	LogData.BlueprintId = BlueprintId;

	if (UBFLogging::ShouldPrintLogsInstant())
		LogData.PrintLog();
	
	Logs.Add(LogData);
}

FUBFExecutionReport FUBFLogData::CreateReport() const
{
	FUBFExecutionReport Report;
	Report.Logs = Logs;
	Report.bWasSuccessful = WasSuccessful();
	Report.Summary = CreateReportSummary();
	return Report;
}

bool FUBFLogData::WasSuccessful() const
{
	for (const auto& Log : Logs)
	{
		if (Log.Level == EUBFLogLevel::Error)
			return false;
	}

	return true;
}

FString FUBFLogData::CreateReportSummary() const
{
	TMap<EUBFLogLevel, int> Count;
	for (const auto& Log : Logs)
	{
		Count.FindOrAdd(Log.Level)++;
	}
	
	return FString::Printf(TEXT("Root Blueprint: {%s} finished executing with %d Errors, %d Warnings, %d Logs."), *RootBlueprintId,
		Count.FindOrAdd(EUBFLogLevel::Error), Count.FindOrAdd(EUBFLogLevel::Warning), Count.FindOrAdd(EUBFLogLevel::Log));
}
