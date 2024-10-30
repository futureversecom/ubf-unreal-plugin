// Fill out your copyright notice in the Description page of Project Settings.


#include "UBFAPISubsystem.h"

#include "Graph.h"
#include "HttpModule.h"
#include "UBFLog.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Async/Async.h"

UUBFAPISubsystem* UUBFAPISubsystem::Get(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	if (!WorldContext->GetWorld()) return nullptr;
	if (!WorldContext->GetWorld()->GetGameInstance()) return nullptr;
	
	return WorldContext->GetWorld()->GetGameInstance()->GetSubsystem<UUBFAPISubsystem>();
}

UUBFGraphReference* UUBFAPISubsystem::FindLoadedGraph(const FString& GraphId)
{
	for (const auto Graph : LoadedGraphs)
	{
		if (Graph->GetBlueprintId() == GraphId)
		{
			return Graph;
		}
	}
	return nullptr;
}

TFuture<bool> UUBFAPISubsystem::UpdateManifestAsync(const FString& BlueprintId)
{
	TSharedPtr<TPromise<bool>> Promise = MakeShareable(new TPromise<bool>());
	TFuture<bool> Future = Promise->GetFuture();

	UUBFGraphReference* GraphRef = FindLoadedGraph(BlueprintId);
	if (GraphRef == nullptr)
	{
		UE_LOG(LogUBF, Error, TEXT("No matching loaded graph"));
		Promise->SetValue(false);
	}
	
	FString Path = GraphRef->GetURI().Replace(TEXT("ubf.json"), TEXT("manifest.json"));
	
	MakeDownloadRequest(Path).Next([this, Promise, BlueprintId](TSharedPtr<IHttpResponse> Response)
	{
		if (Response == nullptr || !Response.IsValid())
		{
			UE_LOG(LogUBF, Error, TEXT("%s Manifest download failure"), *BlueprintId);
			Promise->SetValue(false);
			return;
		}
		
		if (Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
		{
			TSharedPtr<FResourceManifest> Manifest = FResourceManifest::FromJSON(Response->GetContentAsString());
			ResourceManifestMap.FindOrAdd(BlueprintId, Manifest);
			ResourceManifestRecordMap.FindOrAdd(BlueprintId, FDateTime::Now());
			
			UE_LOG(LogUBF, Verbose, TEXT("%s Manifest download success for %d items"), *BlueprintId, Manifest->ResourceManifestElements.Num());
			Promise->SetValue(true);
		}
		else
		{
			FString ErrorMessage = GetDescription(static_cast<EHttpResponseCodes::Type>(Response->GetResponseCode())).ToString();
			UE_LOG(LogUBF, Error, TEXT("%s Manifest download failure. Message: %s"), *BlueprintId, *ErrorMessage);
			Promise->SetValue(false);
		}
	});
	
	return Future;
}

bool UUBFAPISubsystem::ManifestRequiresUpdate(const FString& BlueprintId)
{
	if (!ResourceManifestRecordMap.Contains(BlueprintId))
		return true;
	
	FDateTime LastUpdated = ResourceManifestRecordMap[BlueprintId];
	FTimespan Span = FDateTime::Now() - LastUpdated;
	bool bRequiresUpdate = (Span.GetTotalSeconds() > ManifestUpdateFrequency);
	return bRequiresUpdate;
}

TSharedPtr<FResourceManifest> UUBFAPISubsystem::GetManifest(const FString& BlueprintId)
{
	if (!ResourceManifestMap.Contains(BlueprintId)) return nullptr;
	return ResourceManifestMap[BlueprintId];
}

void UUBFAPISubsystem::SaveResourceHash(const FString& BlueprintId, const FString& ResourceId, const FString& Hash)
{
	const auto ResourceKey = FString::Printf(TEXT("Id: %s Uri: %s"), *BlueprintId, *ResourceId);
	ResourceHashesMap.FindOrAdd(ResourceKey, Hash);
}

FString UUBFAPISubsystem::GetResourceHash(const FString& BlueprintId, const FString& ResourceId)
{
	const auto ResourceKey = FString::Printf(TEXT("Id: %s Uri: %s"), *BlueprintId, *ResourceId);
	if(ResourceHashesMap.Contains(ResourceKey))
	{
		return ResourceHashesMap[ResourceKey];
	}
	return "";
}

TFuture<bool> UUBFAPISubsystem::DownloadGraphAsync(const FString& GraphURI)
{
	TSharedPtr<TPromise<bool>> Promise = MakeShareable(new TPromise<bool>());
	TFuture<bool> Future = Promise->GetFuture();
	
	UUBFGraphReference* GraphRef = NewObject<UUBFGraphReference>(this);

	// Prevent GC of graphs before being assigned as loaded
	LoadingGraphs.Add(GraphRef);
	
	GraphRef->InitAsync(GraphURI, ServerLocation).Next([this, GraphRef, Promise, GraphURI](bool bSuccess)
	{
		if (bSuccess)
		{
			UE_LOG(LogUBF, Verbose, TEXT("Downloaded %s Graph Successfully"), *GraphRef->GetBlueprintId());
			LoadingGraphs.Remove(GraphRef);
			LoadedGraphs.Add(GraphRef);
			
			UpdateManifestAsync(GraphRef->GetBlueprintId()).Next([this, Promise](bool bSuccess)
			{
				Promise->SetValue(bSuccess);
			});
		}
		else
		{
			UE_LOG(LogUBF, Error, TEXT("Failed to Download %s Graph"), *GraphURI);
			LoadingGraphs.Remove(GraphRef);
			Promise->SetValue(false);
		}
	});

	return Future;
}

void UUBFAPISubsystem::UpdateGraphsFromLocalURI(const FString& Location, const FOnComplete& OnComplete)
{
	if (!FPaths::DirectoryExists(Location))
	{
		UE_LOG(LogUBF, Error, TEXT("[API] Cannot update graphs from local URI: directory does not exist"));
		return;
	}

	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *Location, TEXT("*ubf.json"), true, false);

	UE_LOG(LogUBF, Verbose, TEXT("Files: #%d"), Files.Num());

	for (FString& File : Files)
	{
		File = File.Replace(*Location, TEXT("")).Replace(TEXT("ubf.json"), TEXT(""));
	}

	for (auto GraphUri : Files)
	{
		DownloadGraphAsync(GraphUri).Next([this, Files, OnComplete](bool bSuccess)
		{
			if (LoadedGraphs.Num() == Files.Num())
			{
				OnComplete.ExecuteIfBound();
			}
		});
	}
}

TFuture<TSharedPtr<IHttpResponse>> UUBFAPISubsystem::MakeDownloadRequest(const FString& Path)
{
	TSharedPtr<TPromise<TSharedPtr<IHttpResponse>>> Promise = MakeShareable(new TPromise<TSharedPtr<IHttpResponse>>());
	TFuture<TSharedPtr<IHttpResponse>> Future = Promise->GetFuture();
	
	const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	// encode space character
	FString EncodedUrl = Path.Replace(TEXT(" "), TEXT("%20"));
	Request->SetURL(EncodedUrl);
	Request->SetVerb(TEXT("GET"));
	
	auto Callback = [Promise](FHttpRequestPtr Request, const FHttpResponsePtr& Response, bool bWasSuccessful) mutable
	{
		if (Response == nullptr)
		{
			Promise->SetValue(nullptr);
			return;
		}
		
		if (bWasSuccessful && Response.IsValid())
		{
			Promise->SetValue(Response);
		}
		else
		{
			Promise->SetValue(nullptr);
		}
	};
	
	Request->OnProcessRequestComplete().BindLambda(Callback);
	Request->ProcessRequest();

	return Future;
}

TFuture<bool> UUBFGraphReference::DownloadGraphDataAsync()
{
	TSharedPtr<TPromise<bool>> Promise = MakeShareable(new TPromise<bool>());
	TFuture<bool> Future = Promise->GetFuture();
	
	UUBFAPISubsystem::MakeDownloadRequest(URI).Next([this, Promise](TSharedPtr<IHttpResponse> Response)
	{
		if (Response == nullptr)
		{
			UE_LOG(LogUBF, Error, TEXT("%s DownloadGraphData Failure"), *URI);
			Promise->SetValue(false);
			return;
		}
		
		if (Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Ok)
		{
			Json = Response->GetContentAsString();
			UBF::FGraphHandle Graph;
			
			if (UBF::FGraphHandle::Load(UBF::FRegistryHandle::Default(), Json, Graph))
			{
				//BlueprintId = Graph.GetTempId();
				Promise->SetValue(true);
			}
			else
			{
				Promise->SetValue(false);
			}
		}
		else
		{
			Promise->SetValue(false);
		}
	});

	return Future;
}

bool UUBFGraphReference::TryGetGraph(UBF::FGraphHandle& Graph) const
{
	bool Success = UBF::FGraphHandle::Load(UBF::FRegistryHandle::Default(), Json, Graph);
	return Success;
}
