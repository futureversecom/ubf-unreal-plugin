// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "BlueprintUBFLibrary.h"

#include "ExecutionSets/IExecutionSetConfig.h"
#include "ExecutionSets/IExecutionSetData.h"
#include "ExecutionSets/ExecutionInstance.h"
#include "ExecutionSets/ExecutionSetResult.h"

UBF::FExecutionSetHandle UBF::Execute(const FString& RootId, const TSharedPtr<const IExecutionSetData>& ExecutionSetData)
{
	check(ExecutionSetData.IsValid());
	
	TSharedPtr<IExecutionSetConfig> ExecutionSetConfig = ExecutionSetData->CreateExecutionSetConfig();
	ExecutionSetConfig->SetRootId(RootId);
	TSharedPtr<FExecutionSetResult> SetResult = MakeShared<FExecutionSetResult>();
	
	ExecutionSetConfig->GetExecutionInstance(RootId).Next([ExecutionSetConfig, ExecutionSetData, SetResult](const FLoadExecutionInstanceResult& Result)
	{
		if (!Result.bSuccess)
		{
			auto OnCompleteFunc = ExecutionSetData->GetOnComplete();
			SetResult->SetResults(false, FUBFExecutionReport::Failure());
			OnCompleteFunc(false, SetResult);
			return;
		}
		
		TSharedPtr<FExecutionInstance> ExecutionInstance = Result.Value;
		
		SetResult->SetGraphHandle(ExecutionInstance->GetGraphHandleRef());
		
		auto OnCompleteFunc = ExecutionSetData->GetOnComplete();
		// TODO refactor how FUBFExecutionReport gets returned, we should do something to do with FExecutionSetResult
		auto OnComplete = [SetResult, ExecutionSetConfig, OnCompleteFunc](bool bSuccess, FUBFExecutionReport ExecutionReport)
		{
			SetResult->SetResults(bSuccess, ExecutionReport);
			OnCompleteFunc(bSuccess, SetResult);
		};
		
		auto OnNodeStart = [](FString, FFI::ScopeID){};
		auto OnNodeComplete = [](FString, FFI::ScopeID){};
		
		ExecutionInstance->Execute(ExecutionSetConfig, OnComplete, OnNodeStart, OnNodeComplete,SetResult->GetMutableExecutionContext());
	});

	return FExecutionSetHandle(ExecutionSetConfig, SetResult);
}
