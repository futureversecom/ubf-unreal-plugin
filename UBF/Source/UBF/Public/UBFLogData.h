#pragma once
#include "UBFExecutionReport.h"


class FUBFLogData
{
public:
	void Log(const FString& BlueprintId, EUBFLogLevel Level, const FString& Log);
private:
	TMap<FString, FBlueprintLogData> LogMap;
};
