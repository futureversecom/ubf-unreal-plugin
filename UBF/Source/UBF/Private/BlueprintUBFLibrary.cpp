// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "BlueprintUBFLibrary.h"

#include "UBFRuntimeController.h"
#include "Util/UBFUtils.h"
#include "ExecutionSets/IExecutionSetConfig.h"
#include "ExecutionSets/IExecutionSetData.h"
#include "ExecutionSets/ExecutionInstance.h"
#include "ExecutionSets/ExecutionSetData.h"
#include "ExecutionSets/ExecutionSetResult.h"
#include "GlobalArtifactProvider/DownloadRequestManager.h"
#include "GlobalArtifactProvider/GlobalArtifactProviderSubsystem.h"
#include "Util/CatalogUtils.h"

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
		
		// TODO refactor how FUBFExecutionReport gets returned, we should do something to do with FExecutionSetResult
		auto OnComplete = [SetResult, ExecutionSetConfig, ExecutionSetData](bool bSuccess, FUBFExecutionReport ExecutionReport)
		{
			auto OnCompleteFunc = ExecutionSetData->GetOnComplete();
			SetResult->SetResults(bSuccess, ExecutionReport);
			OnCompleteFunc(bSuccess, SetResult);
		};
		
		auto OnNodeStart = [](FString, FFI::ScopeID){};
		auto OnNodeComplete = [](FString, FFI::ScopeID){};
		
		ExecutionInstance->Execute(ExecutionSetConfig, OnComplete, OnNodeStart, OnNodeComplete,SetResult->GetMutableExecutionContext());
	});

	return FExecutionSetHandle(ExecutionSetConfig, SetResult);
}

UUBFBindingObject* UBlueprintUBFLibrary::CreateNewInputBindingObject(const FString& Id, const FString& Type,
	const FString& Value)
{
	return UBFUtils::CreateNewInputBindingObject(Id, Type, Value);
}

void UBlueprintUBFLibrary::RegisterCatalogs(const UObject* WorldContextObject, const FString& CatalogPath,
	const bool UseRelativePath, FOnRegisterCatalogComplete OnComplete)
{
	FString ConvertedPath = CatalogPath.Replace(TEXT("\\"), TEXT("/"));
	FDownloadRequestManager::GetInstance()->LoadStringFromURI(TEXT("Catalog"), ConvertedPath)
		.Next([WorldContextObject, ConvertedPath, UseRelativePath, OnComplete](const UBF::FLoadStringResult& LoadResult)
		{
			if (!LoadResult.bSuccess)
			{
				UE_LOG(LogUBF, Warning, TEXT("Failed to load parsing catalog from %s"), *ConvertedPath);
				return;
			}
			
			TMap<FString, UBF::FCatalogElement> CatalogMap;
			const auto RelativePath = UseRelativePath ? FPaths::GetPath(ConvertedPath) : "";
			CatalogUtils::ParseCatalog(LoadResult.Value, CatalogMap, RelativePath);
			UE_LOG(LogUBF, Verbose, TEXT("Adding catalog from Path: %s"), *ConvertedPath);
			
			UGlobalArtifactProviderSubsystem::Get(WorldContextObject)->RegisterCatalogs(CatalogMap);

			if (OnComplete.IsBound())
				OnComplete.Execute();
		});
}

void UBlueprintUBFLibrary::ExecuteBlueprint(UUBFRuntimeController* RuntimeController, const FString& BlueprintId,
	const TMap<FString, UUBFBindingObject*>& InputMap, const FOnComplete& OnComplete)
{
	const UBF::FExecutionInstanceData BlueprintData(BlueprintId);
	
	FBlueprintExecutionData ExecutionData;
	ExecutionData.BlueprintInstances.Add(BlueprintData);
	ExecutionData.InputMap = InputMap;
	
	RuntimeController->ExecuteBlueprint(BlueprintData.GetInstanceId(), ExecutionData, OnComplete);
}
