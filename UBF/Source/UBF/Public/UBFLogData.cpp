#include "UBFLogData.h"


void FUBFLogData::Log(const FString& BlueprintId, EUBFLogLevel Level, const FString& Log)
{
	FLogData LogData;
	LogData.Level = Level;
	LogData.Log = Log;
	
	LogMap.FindOrAdd(BlueprintId).LogDatas.Add(LogData);
}
