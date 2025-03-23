// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "UBF.h"

#define LOCTEXT_NAMESPACE "FUBFModule"

#include "UBFLog.h"
#include "ubf_interpreter.h"
#include "Interfaces/IPluginManager.h"

static void UELogCallback(const char* Bytes)
{
	const size_t n = strlen(Bytes);
	const FString Str = FString(n, Bytes);

	// Convert the FString to lowercase for case-insensitive comparison
	FString LowerStr = Str.ToLower();

	// Determine log level based on keywords
	ELogVerbosity::Type LogLevelToUse = ELogVerbosity::VeryVerbose;

	if (LowerStr.Contains(TEXT("fail")) || LowerStr.Contains(TEXT("error")))
	{
		LogLevelToUse = ELogVerbosity::Error;
	}
	else if (LowerStr.Contains(TEXT("warn")))
	{
		LogLevelToUse = ELogVerbosity::Warning;
	}

	// Log with the determined log level
	switch (LogLevelToUse)
	{
	case ELogVerbosity::Error:
		UE_LOG(LogUBF, Error, TEXT("[NativeLog] %s"), *Str);
		break;
	case ELogVerbosity::Warning:
		UE_LOG(LogUBF, Warning, TEXT("[NativeLog] %s"), *Str);
		break;
	default:
		UE_LOG(LogUBF, VeryVerbose, TEXT("[NativeLog] %s"), *Str);
		break;
	}
}

void FUBFModule::StartupModule()
{
	const auto Plugin = IPluginManager::Get().FindPlugin(TEXT("UBF"));
	if (!Plugin.IsValid())
	{
		UE_LOG(LogUBF, Error, TEXT("[UBF] Dll open failed"));
		return;
	}
	
	FString PluginBaseDir = Plugin->GetBaseDir();
	const FString DLLPath = FPaths::Combine(PluginBaseDir, TEXT("Binaries"), TEXT("Win64"), TEXT("ubf_interpreter.dll"));
	UE_LOG(LogUBF, Log, TEXT("[UBF] Loading Plugin from %s"), *DLLPath);
	
	UE_LOG(LogUBF, Log, TEXT("[UBF] Loading Plugin"));
	DLLHandle = FPlatformProcess::GetDllHandle(*DLLPath);
	if (DLLHandle == nullptr)
	{
		UE_LOG(LogUBF, Error, TEXT("[UBF] Dll open failed"));
	}
	else {
		UE_LOG(LogUBF, Log, TEXT("[UBF] DLL loaded"));
		CALL_RUST_FUNC(init_ue_logger)(UELogCallback);
	}
}


void FUBFModule::ShutdownModule()
{
	if (DLLHandle != nullptr) {
		FPlatformProcess::FreeDllHandle(DLLHandle);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUBFModule, UBF)