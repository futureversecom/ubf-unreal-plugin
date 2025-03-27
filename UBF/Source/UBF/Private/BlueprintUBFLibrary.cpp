// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "BlueprintUBFLibrary.h"

#include "ExecutionSets/IExecutionSetConfig.h"
#include "ExecutionSets/IExecutionSetData.h"
#include "ExecutionSets/ExecutionInstance.h"

void UBF::Execute(const FString& RootId, const TSharedPtr<IExecutionSetData>& ExecutionSetData)
{
	check(ExecutionSetData.IsValid());
	
	TSharedPtr<IExecutionSetConfig> ExecutionSetConfig = ExecutionSetData->CreateExecutionSetConfig();

	TSharedPtr<FExecutionInstance> ExecutionInstance = ExecutionSetConfig->GetExecutionInstance(RootId);
	
	FExecutionContextHandle ExecutionContext;

	ExecutionInstance->Execute(ExecutionSetConfig, ExecutionSetData->GetOnComplete(),ExecutionContext);

	ExecutionSetData->SetExecutionContext(ExecutionContext);
}
