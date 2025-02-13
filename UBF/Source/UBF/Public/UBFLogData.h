#pragma once
#include "UBFExecutionReport.h"


class UBF_API FUBFLogData
{
public:
	FUBFLogData() {}
	FUBFLogData(const FString& RootBlueprintId) : RootBlueprintId(RootBlueprintId) {}
	void Log(const FString& BlueprintId, EUBFLogLevel Level, const FString& Log);

	FUBFExecutionReport CreateReport() const;
	
	bool WasSuccessful() const;
	
	FString CreateReportSummary() const;;
	FString GetRootBlueprintId() const {return RootBlueprintId;}
private:
	TArray<FLogData> Logs;
	FString RootBlueprintId;
};
