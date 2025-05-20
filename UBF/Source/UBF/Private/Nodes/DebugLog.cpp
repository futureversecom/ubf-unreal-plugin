// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "Nodes/DebugLog.h"

#include "UBFLog.h"
namespace UBF
{
	bool FDebugLog::ExecuteSync() const
	{
		FString Message;
		if (TryReadInputValue(TEXT("Message"), Message))
		{
			UBF_LOG(Log, TEXT("%s"), *(FString)(*Message));
		}

		TriggerNext();
		return true;
	}
}
