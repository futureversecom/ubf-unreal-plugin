#include "ExecuteBlueprint2Node.h"

#include "GraphProvider.h"
#include "SubGraphResolver.h"

void UBF::FExecuteBlueprint2Node::ExecuteAsync() const
{
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

	if (GetContext().BlueprintInstanceExistsForId(BlueprintId))
	{
		FBlueprintInstance BlueprintInstance = GetContext().GetInstanceForId(BlueprintId);
		UBF_LOG(Verbose, TEXT("[ExecuteBlueprint2Node] Found blueprint instance for BlueprintId: '%s' BlueprintId will be replaced with '%s'"), *BlueprintId, *BlueprintInstance.GetBlueprintId());
		BlueprintId = BlueprintInstance.GetBlueprintId();
		ActualInputs = BlueprintInstance.GetInputs();
	}
	
	for (auto ExpectedInput : ExpectedInputs)
	{
		if (ExpectedInput == "Blueprint") continue;
		FDynamicHandle DynamicOutput;
		if (TryReadInput(ExpectedInput, DynamicOutput))
		{
			ActualInputs.Add(ExpectedInput, DynamicOutput);
		}
	}
	
	UBF_LOG(Verbose, TEXT("[ExecuteBlueprint2Node] NodeInputs Count %d for BlueprintId: '%s'"), ExpectedInputs.Num(), *BlueprintId);

	GetContext().GetGraphProvider()->GetGraph(BlueprintId).Next([this, BlueprintId, ActualInputs](const FLoadGraphResult& Result)
	{
		if (!Result.Result.Key)
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
			Result.Result.Value.GetOutputs(Outputs);
			
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

		const FGraphHandle Graph = Result.Result.Value;
		Graph.Execute(BlueprintId, GetContext().GetRoot()->GetAttachmentComponent(), GetContext().GetGraphProvider(), GetContext().GetUserData()->LogData, GetContext().GetUserData()->InstancedBlueprints,
			ActualInputs, OnCompleteFunc, ExecContext);
	});
}
