// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "ExecuteBlueprint2Node.h"

#include "BlueprintUBFLibrary.h"
#include "GraphProvider.h"
#include "SubGraphResolver.h"

void UBF::FExecuteBlueprint2Node::ExecuteAsync() const
{
	UBF_LOG(Verbose, TEXT("[ExecuteBlueprint2Node] Executing: '%s'"), *GetContext().GetBlueprintID());
	
	FString BlueprintId;
	if (!TryReadInputValue("Blueprint", BlueprintId))
	{
		UBF_LOG(Warning, TEXT("[ExecuteBlueprint2Node] Failed to read Key Input"));
		TriggerNext();
		CompleteAsyncExecution();
		return;
	}

	if (BlueprintId.IsEmpty())
	{
		UBF_LOG(Verbose, TEXT("[ExecuteBlueprint2Node] BlueprintId found was empty on graph: '%s'"), *GetContext().GetBlueprintID());
		TriggerNext();
		CompleteAsyncExecution();
		return;
	}
	
	TArray<FString> ExpectedInputs;
	GetContext().GetDeclaredNodeInputs(GetNodeId(), ExpectedInputs);
	
	TMap<FString, FDynamicHandle> ActualInputs;
	
	for (auto ExpectedInput : ExpectedInputs)
	{
		if (ExpectedInput == "Blueprint") continue;
		FDynamicHandle DynamicOutput;
		if (TryReadInput(ExpectedInput, DynamicOutput))
		{
			ActualInputs.Add(ExpectedInput, DynamicOutput);
		}
	}

	GetContext().GetUserData()->ExecutionSetConfig->GetExecutionInstance(BlueprintId).Next([this, BlueprintId, ActualInputs](const FLoadExecutionInstanceResult& Result)
	{
		if (!CheckExecutionStillValid())
		{
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}
		
		if (!Result.bSuccess)
		{
			UBF_LOG(Error, TEXT("[ExecuteBlueprint2Node] Aborting execution: graph '%s' is invalid"), *BlueprintId);
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}
		
		auto OnCompleteFunc = [Result, this, BlueprintId](bool Success, FUBFExecutionReport ExecutionReport)
		{
			UBF_LOG(Verbose, TEXT("[ExecuteBlueprint2Node] Completed Executing Graph '%s'"), *BlueprintId);
	
			TArray<FBindingInfo> Outputs;
			Result.Value->GetGraphHandleRef().GetOutputs(Outputs);
			
			UBF_LOG(Verbose, TEXT("[ExecuteBlueprint2Node] Output Count %d"), Outputs.Num());
			
			for (auto Output : Outputs)
			{
				UBF_LOG(Verbose, TEXT("[ExecuteBlueprint2Node] Looking up Output from subgraph. Binding Id: '%s'"), *Output.Id);
					
				FDynamicHandle Dynamic;
				if (ExecContext.TryReadOutput(Output.Id, Dynamic))
				{
					UBF_LOG(Verbose, TEXT("Writing Output: Id: '%s' Value: %s"), *Output.Id, *Dynamic.ToString());
					WriteOutput(Output.Id, Dynamic);
				}
			}
					
			TriggerNext();
			CompleteAsyncExecution();
		};

		auto OnNodeStart = [](FString, FFI::ScopeID){};
		auto OnNodeComplete = [](FString, FFI::ScopeID){};
		
		Result.Value->ExecuteWithInputs(GetContext().GetUserData()->ExecutionSetConfig,
			MoveTemp(OnCompleteFunc), MoveTemp(OnNodeStart), MoveTemp(OnNodeComplete), ActualInputs, ExecContext);
	});
}
