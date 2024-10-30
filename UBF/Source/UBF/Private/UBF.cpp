#include "UBF.h"

#define LOCTEXT_NAMESPACE "FUBFModule"

#include "UBFLog.h"
#include "ubf_interpreter.h"

static void UELogCallback(const char* Bytes)
{
	const size_t n = strlen(Bytes);
    const FString& Str = FString(n, Bytes);
	UE_LOG(LogUBF, VeryVerbose, TEXT("%s"), *Str);
}

void FUBFModule::StartupModule()
{
	const FString DLLName = "ubf_interpreter.dll";

	UE_LOG(LogUBF, Log, TEXT("[UBF] Loading Plugin"));
	DLLHandle = FPlatformProcess::GetDllHandle(*DLLName);
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