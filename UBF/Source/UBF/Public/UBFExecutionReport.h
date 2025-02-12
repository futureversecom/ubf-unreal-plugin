// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UBFExecutionReport.generated.h"

UENUM(BlueprintType)
enum class EUBFLogLevel
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
public:
	UPROPERTY(BlueprintReadWrite)
	EUBFLogLevel Level;
	UPROPERTY(BlueprintReadWrite)
	FString Log;
};

USTRUCT(BlueprintType)
struct FBlueprintLogData
{
public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FLogData> LogDatas;
};

#define UBF_LOG(LogLevel, Format, ...) \
do { \
{ \
FString FormattedMessage = FString::Printf(Format, ##__VA_ARGS__); \
Log(EUBFLogLevel::LogLevel, FormattedMessage); \
} \
} while (0)

/**
 * 
 */
USTRUCT(BlueprintType)
struct UBF_API FUBFExecutionReport
{
	GENERATED_BODY()
public:
	
};
