// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "UBFExecutionReport.h"

#include "UBFLog.h"

void FLogData::PrintLog() const
{
	const FString FormattedLog = FString::Printf(TEXT("[%s] %s"), *BlueprintId, *Log);

	switch (Level)
	{
	case EUBFLogLevel::VeryVerbose:
		UE_LOG(LogUBF, VeryVerbose, TEXT("%s"), *FormattedLog);
		break;
	case EUBFLogLevel::Verbose:
		UE_LOG(LogUBF, Verbose, TEXT("%s"), *FormattedLog);
		break;
	case EUBFLogLevel::Log:
		UE_LOG(LogUBF, Log, TEXT("%s"), *FormattedLog);
		break;
	case EUBFLogLevel::Warning:
		UE_LOG(LogUBF, Warning, TEXT("%s"), *FormattedLog);
		break;
	case EUBFLogLevel::Error:
		UE_LOG(LogUBF, Error, TEXT("%s"), *FormattedLog);
		break;
	default:
		UE_LOG(LogUBF, Log, TEXT("%s"), *FormattedLog);
		break;
	}
}
