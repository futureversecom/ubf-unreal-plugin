#include "DebugLog.h"
#include "UBFLog.h"
namespace UBF
{
	bool FDebugLog::ExecuteSync() const
	{
		FString Message;
		if (TryReadInputValue(TEXT("Message"), Message))
		{
			UE_LOG(LogUBF, Log, TEXT("%s"), *(FString)(*Message));
		}

		TriggerNext();
		return true;
	}
}
