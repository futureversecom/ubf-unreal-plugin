#include "ExecutionSets/ExecutionSetResult.h"

namespace UBF
{
	void FExecutionSetResult::SetGraphHandle(const FGraphHandle& InGraphHandle)
	{
		RootGraphHandle = InGraphHandle;
		bIsGraphSet = true;
	}

	void FExecutionSetResult::SetResults(bool bSuccess, const FUBFExecutionReport& InExecutionReport)
	{
		ExecutionReport = InExecutionReport;
		bWasSuccessful = bSuccess;
		bIsComplete = true;
	}

	bool FExecutionSetResult::TryReadOutput(const FString& OutputId, FDynamicHandle& OutValue) const
	{
		return ExecutionContextHandle.TryReadOutput(OutputId, OutValue);
	}

	bool FExecutionSetResult::TryReadOutputString(const FString& OutputId, FString& OutValue) const
	{
		FDynamicHandle DynamicHandle;
		if (!TryReadOutput(OutputId, DynamicHandle)) return false;

		return DynamicHandle.TryInterpretAs(OutValue);
	}

	TArray<FString> FExecutionSetResult::GetOutputNames() const
	{
		// Reading invalid graph's outputs causes crash
		TArray<FString> OutputNames;
		if (!bIsGraphSet) return OutputNames;
		
		TArray<FBindingInfo> Outputs;
		RootGraphHandle.GetOutputs(Outputs);

		for (auto Output : Outputs)
		{
			OutputNames.Add(Output.Id);
		}

		return OutputNames;
	}

	TMap<FString, FDynamicHandle> FExecutionSetResult::GetAllOutputs() const
	{
		TMap<FString, FDynamicHandle> Outputs;
		for (const FString& OutputId : GetOutputNames())
		{
			FDynamicHandle DynamicHandle;
			if (TryReadOutput(OutputId, DynamicHandle))
				Outputs.Add(OutputId, DynamicHandle);
		}
		return Outputs;
	}
}
