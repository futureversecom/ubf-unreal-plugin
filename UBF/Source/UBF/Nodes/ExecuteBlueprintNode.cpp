#include "ExecuteBlueprintNode.h"

#include "GraphProvider.h"
#include "SubGraphResolver.h"
#include "UBFAPISubsystem.h"

namespace UBF
{
	void FExecuteBlueprintNode::ExecuteAsync() const
	{
		FString Key;
		if (!TryReadInputValue("Key", Key))
		{
			UE_LOG(LogUBF, Warning, TEXT("[ExecuteBlueprintNode] Failed to read Key Input"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		if (Key.IsEmpty())
		{
			UE_LOG(LogUBF, Warning, TEXT("[ExecuteBlueprintNode] Empty Key was given for ExecuteBlueprintNode"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}
		
		TArray<FString> ExpectedInputs;
		GetContext().GetDeclaredNodeInputs(GetNodeId(), ExpectedInputs);
		
		TMap<FString, FDynamicHandle> ActualInputs;
		for (auto ExpectedInput : ExpectedInputs)
		{
			FDynamicHandle DynamicOutput;
			if (TryReadInput(ExpectedInput, DynamicOutput))
			{
				ActualInputs.Add(ExpectedInput, DynamicOutput);
			}
		}
		
		UE_LOG(LogUBF, Verbose, TEXT("[ExecuteBlueprintNode] NodeInputs Count %d for Key: %s"), ExpectedInputs.Num(), *Key);
		
		TMap<FString, FDynamicHandle> ResolvedInputs;
		FString BlueprintId;
		if (!GetContext().GetSubGraphResolver()->TryResolveSubGraph(Key, ActualInputs, BlueprintId, ResolvedInputs))
		{
			UE_LOG(LogUBF, Warning, TEXT("[ExecuteBlueprintNode] Unable to resolve subgraph for key %s"), *Key);
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		UE_LOG(LogUBF, Verbose, TEXT("[ExecuteBlueprintNode] Executing Key: %s BlueprintId: %s UBF"), *Key, *BlueprintId);
		
		GetContext().GetGraphProvider()->GetGraph(BlueprintId).Next([this, ResolvedInputs, BlueprintId](const FLoadGraphResult& Result)
		{
			if (!Result.Result.Key)
			{
				UE_LOG(LogUBF, Error, TEXT("[ExecuteBlueprintNode] Aborting execution: graph '%s' is invalid"), *BlueprintId);
				TriggerNext();
				CompleteAsyncExecution();
				return;
			}
			
			auto OnCompleteFunc = [Result, this, BlueprintId]
			{
				UE_LOG(LogUBF, Verbose, TEXT("[ExecuteBlueprintNode] Completed Executing Graph %s"), *BlueprintId);
		
				TArray<FBindingInfo> Outputs;
				Result.Result.Value.GetOutputs(Outputs);
				
				UE_LOG(LogUBF, Verbose, TEXT("[ExecuteBlueprintNode] Output Count %d"), Outputs.Num());
				
				for (auto Output : Outputs)
				{
					UE_LOG(LogUBF, Warning, TEXT("[ExecuteBlueprintNode] Looking up Output from subgraph. Binding Id: %s"), *Output.Id);
						
					FDynamicHandle Dynamic;
					if (ExecContext.TryReadOutput(Output.Id, Dynamic))
					{
						// IMPORTANT We must prepend the 'Out.' prefix to the output
						// id to match the expected output binding when forwarding.
						UE_LOG(LogUBF, Verbose, TEXT("Writing Output: Id: %s Value: %s"), *Output.Id, *Dynamic.ToString())
						WriteOutput("Out." + Output.Id, Dynamic);
					}
				}
						
				TriggerNext();
				CompleteAsyncExecution();
			};

			const FGraphHandle Graph = Result.Result.Value;
			Graph.Execute(BlueprintId, GetContext().GetRoot()->GetAttachmentComponent(), GetContext().GetGraphProvider(), GetContext().GetSubGraphResolver(),
				ResolvedInputs, OnCompleteFunc, ExecContext);
		});
	}
}
