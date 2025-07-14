// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "UBF.h"

#define LOCTEXT_NAMESPACE "FUBFModule"

#include "UBFLog.h"
#include "ubf_interpreter.h"
#include "Interfaces/IPluginManager.h"

static void UELogCallback(const uintptr_t UBFLogLevel, const char *Bytes, UBF::FFI::ArcExecutionContext* Context)
{
	// todo: make use of context
	
	const size_t n = strlen(Bytes);
	const FString Str = FString(n, Bytes);
	
	// Log with the determined log level
	switch (UBFLogLevel)
	{
		case 1:
			UE_LOG(LogUBF, Warning, TEXT("[NativeLog] %s"), *Str);
			break;
		case 2:
			UE_LOG(LogUBF, Error, TEXT("[NativeLog] %s"), *Str);
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
		UE_LOG(LogUBF, Error, TEXT("[UBF] Plugin not found"));
		return;
	}

	FString PluginBaseDir = Plugin->GetBaseDir();
	FString DLLPath;

#if PLATFORM_WINDOWS
	DLLPath = FPaths::Combine(PluginBaseDir, TEXT("Binaries"), TEXT("Win64"), TEXT("ubf_interpreter.dll"));
#elif PLATFORM_MAC
	DLLPath = FPaths::Combine(PluginBaseDir, TEXT("Binaries"), TEXT("Mac"), TEXT("libubf_interpreter.dylib"));
#else
	UE_LOG(LogUBF, Error, TEXT("[UBF] Unsupported platform"));
	return;
#endif

	UE_LOG(LogUBF, Log, TEXT("[UBF] Loading Plugin from %s"), *DLLPath);

	DLLHandle = FPlatformProcess::GetDllHandle(*DLLPath);
	if (DLLHandle == nullptr)
	{
		UE_LOG(LogUBF, Error, TEXT("[UBF] Failed to load library"));
	}
	else
	{
		UE_LOG(LogUBF, Log, TEXT("[UBF] Library loaded"));
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