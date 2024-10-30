// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFLog.h"


DEFINE_LOG_CATEGORY(LogUBF);

namespace UBFLogging
{
	static TAutoConsoleVariable<bool> CVarUBFDebugLogging(
	TEXT("UBF.Logging.DebugLogging"),
	false,
	TEXT("Enable verbose debug logging for UBF."));

	bool DebugLoggingEnabled()
	{
		return CVarUBFDebugLogging.GetValueOnAnyThread();
	}
}


