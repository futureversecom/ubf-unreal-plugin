#pragma once
#include "ExecutionContext.h"

namespace UBF
{
	class UBF_API FExecutionSetResult
    {
    public:
    	FExecutionContextHandle& GetMutableExecutionContext() {return ExecutionContextHandle;};
		void SetGraphHandle(const FGraphHandle& GraphHandle);
		void SetResults(bool bSuccess, const FUBFExecutionReport& ExecutionReport);
		
		bool TryReadOutput(const FString& OutputId, FDynamicHandle& OutValue) const;
		bool TryReadOutputString(const FString& OutputId, FString& OutValue) const;
		
		TArray<FString> GetOutputNames() const;
		TMap<FString, FDynamicHandle> GetAllOutputs() const;
		FUBFExecutionReport GetExecutionReport() const {return ExecutionReport;};
		bool WasSuccessful() const {return bWasSuccessful;};

		bool IsComplete() const {return bIsComplete;};
	private:
		FExecutionContextHandle ExecutionContextHandle;
		FGraphHandle RootGraphHandle;
		FUBFExecutionReport ExecutionReport;
		bool bWasSuccessful = false;
		bool bIsComplete = false;
		bool bIsGraphSet = false;
    };
}