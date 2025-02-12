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
			UBF_LOG(Warning, TEXT("[ExecuteBlueprintNode] Failed to read Key Input"));
			TriggerNext();
			CompleteAsyncExecution();
			return;
		}

		if (Key.IsEmpty())
		{
			UBF_LOG(Warning, TEXT("[ExecuteBlueprintNode] Empty Key was given for ExecuteBlueprintNode"));
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
		
		UBF_LOG(Verbose, TEXT("[ExecuteBlueprintNode] NodeInputs Count %d for Key: %s"), ExpectedInputs.Num(), *Key);
		
		TMap<FString, FDynamicHandle> ResolvedInputs;
		FString ArtifactId;
		// if (!GetContext().GetSubGraphResolver()->TryResolveSubGraph(Key, ActualInputs, ArtifactId, ResolvedInputs))
		// {
		// 	UBF_LOG(Warning, TEXT("[ExecuteBlueprintNode] Unable to resolve subgraph for key %s"), *Key);
		// 	TriggerNext();
		// 	CompleteAsyncExecution();
		// 	return;
		// }

		UBF_LOG(Verbose, TEXT("[ExecuteBlueprintNode] Executing Key: %s BlueprintId: %s UBF"), *Key, *ArtifactId);
		
		GetContext().GetGraphProvider()->GetGraph(ArtifactId).Next([this, ArtifactId](const FLoadGraphResult& Result)
		{
			if (!Result.Result.Key)
			{
				UBF_LOG(Error, TEXT("[ExecuteBlueprintNode] Aborting execution: graph '%s' is invalid"), *ArtifactId);
				TriggerNext();
				CompleteAsyncExecution();
				return;
			}
			
			auto OnCompleteFunc = [Result, this, ArtifactId]
			{
				UBF_LOG(Verbose, TEXT("[ExecuteBlueprintNode] Completed Executing Graph %s"), *ArtifactId);
		
				TArray<FBindingInfo> Outputs;
				Result.Result.Value.GetOutputs(Outputs);
				
				UBF_LOG(Verbose, TEXT("[ExecuteBlueprintNode] Output Count %d"), Outputs.Num());
				
				for (auto Output : Outputs)
				{
					UBF_LOG(Verbose, TEXT("[ExecuteBlueprintNode] Looking up Output from subgraph. Binding Id: %s"), *Output.Id);
						
					FDynamicHandle Dynamic;
					if (ExecContext.TryReadOutput(Output.Id, Dynamic))
					{
						// IMPORTANT We must prepend the 'Out.' prefix to the output
						// id to match the expected output binding when forwarding.
						UBF_LOG(Verbose, TEXT("Writing Output: Id: %s Value: %s"), *Output.Id, *Dynamic.ToString())
						WriteOutput("Out." + Output.Id, Dynamic);
					}
				}
						
				TriggerNext();
				CompleteAsyncExecution();
			};

			const FGraphHandle Graph = Result.Result.Value;
			// Graph.Execute(ArtifactId, GetContext().GetRoot()->GetAttachmentComponent(), GetContext().GetGraphProvider(), GetContext().GetSubGraphResolver(),
			// 	ResolvedInputs, OnCompleteFunc, ExecContext);
		});
	}
}
