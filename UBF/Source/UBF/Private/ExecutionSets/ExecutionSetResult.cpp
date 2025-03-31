#include "ExecutionSets/ExecutionSetResult.h"

namespace UBF
{
	void FExecutionSetResult::SetGraphHandle(const FGraphHandle& InGraphHandle)
	{
		RootGraphHandle = InGraphHandle;
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

	TArray<FString> FExecutionSetResult::GetOutputNames() const
	{
		TArray<FBindingInfo> Outputs;
		RootGraphHandle.GetOutputs(Outputs);
		TArray<FString> OutputNames;

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
