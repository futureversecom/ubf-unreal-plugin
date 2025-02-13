// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFLog.h"


DEFINE_LOG_CATEGORY(LogUBF);

namespace UBFLogging
{

	static TAutoConsoleVariable<bool> CVarUBFPrintLogsInstant(
	TEXT("UBF.Logging.PrintLogsInstant"),
	false,
	TEXT("Enable printing ubf log message as they are logged. Regardless of this logs are can be retrieved from FUBFExecutionReport"));

	static TAutoConsoleVariable<bool> CVarUBFPrintSummary(
TEXT("UBF.Logging.PrintLogSummary"),
true,
TEXT("Enable printing ubf log summary on execution complete"));

	
	bool ShouldPrintLogsInstant()
	{
		return CVarUBFPrintLogsInstant.GetValueOnAnyThread();
	}

	bool ShouldPrintLogSummary()
	{
		return CVarUBFPrintSummary.GetValueOnAnyThread();
	}
}


