// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UBFExecutionReport.generated.h"

UENUM(BlueprintType)
enum class EUBFLogLevel : uint8
{
	VeryVerbose,
	Verbose,
	Log,
	Warning,
	Error,
};

USTRUCT(BlueprintType)
struct FLogData
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadWrite)
	EUBFLogLevel Level;
	UPROPERTY(BlueprintReadWrite)
	FString Log;
	UPROPERTY(BlueprintReadWrite)
	FString BlueprintId;

	void PrintLog() const;
};

#define UBF_LOG(LogLevel, Format, ...) \
do { \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
Log(EUBFLogLevel::LogLevel, FormattedMessage); \
} \
} while (0); \

/**
 * 
 */
USTRUCT(BlueprintType)
struct UBF_API FUBFExecutionReport
{
	GENERATED_BODY()
public:
	static FUBFExecutionReport Failure()
	{
		FUBFExecutionReport Report = FUBFExecutionReport();
		Report.bWasSuccessful = false;
		Report.Summary = FString(TEXT("UBFExecution ended in failure"));
		return Report;
	}
	UPROPERTY(BlueprintReadWrite)
	TArray<FLogData> Logs;
	UPROPERTY(BlueprintReadWrite)
	bool bWasSuccessful = true;
	UPROPERTY(BlueprintReadWrite)
	FString Summary;
};
