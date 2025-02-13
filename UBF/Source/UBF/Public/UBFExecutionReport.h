// Fill out your copyright notice in the Description page of Project Settings.

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
	UPROPERTY(BlueprintType)
	TArray<FLogData> Logs;
	UPROPERTY(BlueprintType)
	bool bWasSuccessful = true;
	UPROPERTY(BlueprintType)
	FString Summary;
};
