// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#define INTERNAL_CALL_RUST_FUNC(FunctionName, Lookup) \
	[&]{\
		const FName ModuleName = FName(TEXT("UBF")); \
		const FUBFModule& CurrentModule = FModuleManager::GetModuleChecked<FUBFModule>(ModuleName); \
		using namespace UBF::FFI; \
		using FunctionType = decltype(&FunctionName); \
		static void* FnPtrRaw = FPlatformProcess::GetDllExport(CurrentModule.DLLHandle, TEXT(#Lookup)); \
		if (FnPtrRaw == nullptr) { \
			UE_LOG(LogUBF, Error, TEXT("DLL Export not Found \"" #FunctionName "\"")); \
		} else { \
			/* UE_LOG(LogUBF, VeryVerbose, TEXT("DLL Export found \"" #FunctionName "\"")); */ \
		} \
		static FunctionType FunctionPtr = static_cast<FunctionType>(FnPtrRaw); \
		return FunctionPtr; \
	}()

#define CALL_RUST_FUNC(FunctionName) \
	INTERNAL_CALL_RUST_FUNC(UBF::FFI::FunctionName, FunctionName)

class FUBFModule final : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void* DLLHandle;
};
