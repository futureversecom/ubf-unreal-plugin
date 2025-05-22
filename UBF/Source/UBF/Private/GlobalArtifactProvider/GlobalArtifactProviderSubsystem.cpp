// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "GlobalArtifactProvider/GlobalArtifactProviderSubsystem.h"

#include "glTFRuntimeAsset.h"
#include "ImageUtils.h"
#include "Registry.h"
#include "GlobalArtifactProvider/DownloadRequestManager.h"
#include "GlobalArtifactProvider/CacheLoading/TempCacheLoader.h"
#include "GlobalArtifactProvider/CacheLoading/MemoryCacheLoader.h"
#include "GLTFRuntimeUtils/SpawnGLTFMeshLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Util/FutureUtils.h"

UGlobalArtifactProviderSubsystem::UGlobalArtifactProviderSubsystem()
{
	GraphCacheLoader = MakeShared<FMemoryCacheLoader>();
	ResourceCacheLoader = MakeShared<FTempCacheLoader>();
}

UGlobalArtifactProviderSubsystem* UGlobalArtifactProviderSubsystem::Get(const UObject* WorldContext)
{
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContext))
	{
		return GameInstance->GetSubsystem<UGlobalArtifactProviderSubsystem>();
	}

	return nullptr;
}

TFuture<UBF::FLoadGraphResult> UGlobalArtifactProviderSubsystem::GetGraph(const FString& ArtifactId)
{
	TSharedPtr<TPromise<UBF::FLoadGraphResult>> Promise = MakeShareable(new TPromise<UBF::FLoadGraphResult>());
	TFuture<UBF::FLoadGraphResult> Future = Promise->GetFuture();

	UBF::FLoadGraphResult LoadResult;
	LoadResult.Result = TPair<bool, UBF::FGraphHandle>(false, UBF::FGraphHandle());
	
	// get graph from catalog using graph instance's blueprint id
	if (!Catalog.Contains(ArtifactId))
	{
		UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetGraph Failed to get graph because no graph catalog element found for ArtifactId: %s"), *ArtifactId);
		Promise->SetValue(LoadResult);
		return Future;
	}
	
	const auto GraphResource = Catalog[ArtifactId];
	UE_LOG(LogUBF, Verbose, TEXT("Try Loading Graph %s from Uri %s"), *ArtifactId, *GraphResource.Uri);
	
	FDownloadRequestManager::GetInstance()->LoadStringFromURI(TEXT("Graph"),GraphResource.Uri, GraphResource.Hash, GraphCacheLoader)
	.Next([this, ArtifactId, Promise](const UBF::FLoadStringResult& LoadGraphResult)
	{
		UBF::FLoadGraphResult PromiseResult;
		PromiseResult.Result = TPair<bool,UBF::FGraphHandle>(false, UBF::FGraphHandle());
		
		if (!LoadGraphResult.Result.Key)
		{
			Promise->SetValue(PromiseResult);	
			return;
		}
	
		UBF::FGraphHandle Graph;
		if (UBF::FGraphHandle::Load(UBF::FRegistryHandle::Default(), LoadGraphResult.Result.Value, Graph))
		{
			UE_LOG(LogUBF, Verbose, TEXT("Successfully loaded Graph %s"), *ArtifactId);
		
			PromiseResult.Result = TPair<bool,UBF::FGraphHandle>(true, Graph);
			Promise->SetValue(PromiseResult);	
		}
		else
		{
			UE_LOG(LogUBF, Error, TEXT("Unable to load Graph BlueprintId %s"), *ArtifactId);
			Promise->SetValue(PromiseResult);
		}
	});
	
	return Future;
}

TFuture<UBF::FLoadTextureResult> UGlobalArtifactProviderSubsystem::GetTextureResource(const FString& ArtifactId)
{
	TSharedPtr<TPromise<UBF::FLoadTextureResult>> Promise = MakeShareable(new TPromise<UBF::FLoadTextureResult>());
	TFuture<UBF::FLoadTextureResult> Future = Promise->GetFuture();

	if (LoadedTexturesMap.Contains(ArtifactId) && LoadedTexturesMap[ArtifactId].IsValid())
	{
		UBF::FLoadTextureResult LoadResult;
		LoadResult.Result = TPair<bool, UTexture2D*>(true, LoadedTexturesMap[ArtifactId].Get());
		Promise->SetValue(LoadResult);
		return Future;
	}
	
	if (!Catalog.Contains(ArtifactId))
	{
		UBF::FLoadTextureResult LoadResult;
		UE_LOG(LogUBF, Verbose, TEXT("UGlobalArtifactProviderSubsystem::GetTextureResource UBF doesn't have a ResourceId '%s' entry"), *ArtifactId);
		LoadResult.Result = TPair<bool, UTexture2D*>(false, nullptr);
		Promise->SetValue(LoadResult);
		return Future;
	}

	const auto ResourceManifestElement = Catalog[ArtifactId];
	FDownloadRequestManager::GetInstance()->LoadDataFromURI(TEXT("Texture"),ResourceManifestElement.Uri, ResourceManifestElement.Hash, ResourceCacheLoader)
	.Next([this, Promise, ArtifactId](const UBF::FLoadDataArrayResult& DataResult)
	{
		const TArray<uint8> Data = DataResult.Result.Value;
		UBF::FLoadTextureResult LoadResult;
		
		if (Data.Num() == 0 || Data.GetData() == nullptr)
		{
			UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetTextureResource Data is empty"));
			LoadResult.Result = TPair<bool, UTexture2D*>(false, nullptr);
			Promise->SetValue(LoadResult);
			return;
		}

		UTexture2D* Texture = nullptr;

		if (LoadedTexturesMap.Contains(ArtifactId) && LoadedTexturesMap[ArtifactId].IsValid())
		{
			Texture = LoadedTexturesMap[ArtifactId].Get();
		}
		else
		{
			TRACE_CPUPROFILER_EVENT_SCOPE(UGlobalArtifactProviderSubsystem::GetTextureResource_ImportDataAsTexture);
			Texture = FImageUtils::ImportBufferAsTexture2D(Data);
			LoadedTexturesMap.Remove(ArtifactId);
			LoadedTexturesMap.Add(ArtifactId, Texture);
		}
		
		LoadResult.Result = TPair<bool, UTexture2D*>(true, Texture);
		Promise->SetValue(LoadResult);
	});

	return Future;
}

TFuture<UBF::FLoadMeshResult> UGlobalArtifactProviderSubsystem::GetMeshResource(const FString& ArtifactId, const UBF::FMeshImportSettings& ImportSettings)
{
	TSharedPtr<TPromise<UBF::FLoadMeshResult>> Promise = MakeShareable(new TPromise<UBF::FLoadMeshResult>());
	TFuture<UBF::FLoadMeshResult> Future = Promise->GetFuture();

	if (!Catalog.Contains(ArtifactId))
	{
		UBF::FLoadMeshResult LoadResult;
		TArray<uint8> Data;
		UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource UBF doesn't have a ResourceId %s entry."), *ArtifactId);
		LoadResult.Result = TPair<bool, UglTFRuntimeAsset*>(false, nullptr);
		Promise->SetValue(LoadResult);
		return Future;
	}
	
	const auto ResourceManifestElement = Catalog[ArtifactId];
	const auto Uri = ResourceManifestElement.Uri;
	if (LoadedMeshesMap.Contains(ResourceManifestElement.Uri) && LoadedMeshesMap[ResourceManifestElement.Uri].ContainsMesh(ImportSettings))
	{
		UBF::FLoadMeshResult LoadResult;
		LoadResult.Result = TPair<bool, UglTFRuntimeAsset*>(true, LoadedMeshesMap[ResourceManifestElement.Uri].GetMesh(ImportSettings));
		Promise->SetValue(LoadResult);
		UE_LOG(LogUBF, VeryVerbose, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource Found Cached Mesh for %s"), *Uri);
		return Future;
	}
	
	FDownloadRequestManager::GetInstance()->LoadDataFromURI(TEXT("Mesh"),ResourceManifestElement.Uri, ResourceManifestElement.Hash, ResourceCacheLoader)
	.Next([this, Promise, ImportSettings, ArtifactId, Uri](const UBF::FLoadDataArrayResult& DataResult)
	{
		const TArray<uint8> Data = DataResult.Result.Value;
		UBF::FLoadMeshResult LoadResult;
			
		if (Data.Num() == 0 || Data.GetData() == nullptr)
		{
			UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource Data is empty"));
			LoadResult.Result = TPair<bool, UglTFRuntimeAsset*>(false, nullptr);
			Promise->SetValue(LoadResult);
			return;
		}

		UglTFRuntimeAsset* Asset;

		if (LoadedMeshesMap.Contains(Uri) && LoadedMeshesMap[Uri].ContainsMesh(ImportSettings))
		{
			Asset = LoadedMeshesMap[Uri].GetMesh(ImportSettings);
			UE_LOG(LogUBF, VeryVerbose, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource Found Cached Mesh for %s"), *Uri);
		}
		else
		{
			TRACE_CPUPROFILER_EVENT_SCOPE(UGlobalArtifactProviderSubsystem::GetMeshResource_ImportDataAsGLTFRuntimeAsset);

			Asset = NewObject<UglTFRuntimeAsset>();
			Asset->RuntimeContextObject = ImportSettings.ImportConfig.RuntimeContextObject;
			Asset->RuntimeContextString = ImportSettings.ImportConfig.RuntimeContextString;
					
			if (!Asset->LoadFromData(Data.GetData(), Data.Num(), ImportSettings.ImportConfig))
			{
				UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource Failed to Load Mesh from Data %s"), *ArtifactId);
				LoadResult.Result = TPair<bool, UglTFRuntimeAsset*>(false, nullptr);
				Promise->SetValue(LoadResult);
				return;
			}
			
			LoadedMeshesMap.FindOrAdd(Uri).AddOrReplaceMesh(ImportSettings, Asset);
			UE_LOG(LogUBF, VeryVerbose, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource Caching New Mesh for %s"), *Uri);
		}
		
		LoadResult.Result = TPair<bool, UglTFRuntimeAsset*>(true, Asset);
		Promise->SetValue(LoadResult);
	});

	return Future;
}

TFuture<UBF::FLoadMeshLODResult> UGlobalArtifactProviderSubsystem::GetMeshLODResource(
	const TArray<FMeshResource>& MeshResources, const FMeshConfigData& MeshConfigData)
{
	// TODO cache end results
	
	TSharedPtr<TPromise<UBF::FLoadMeshLODResult>> Promise = MakeShareable(new TPromise<UBF::FLoadMeshLODResult>());
	TFuture<UBF::FLoadMeshLODResult> Future = Promise->GetFuture();
	
	// Load all mesh resources
	TArray<TFuture<UBF::FLoadMeshResult>> MeshFutures;
	for (const FMeshResource& MeshResource : MeshResources)
	{
		MeshFutures.Add(GetMeshResource(MeshResource.ArtifactID, UBF::FMeshImportSettings(MeshConfigData.RuntimeConfig)));
	}

	WaitAll(MeshFutures).Next([this, Promise, MeshConfigData, MeshResources](const TArray<UBF::FLoadMeshResult>& Results)
	{
		TArray<FglTFRuntimeMeshLOD> LODs;
		LODs.SetNum(Results.Num());

		for (int i = 0; i < Results.Num(); i++)
		{
			if (!Results[i].Result.Key)
			{
				UBF::FLoadMeshLODResult LoadMeshLODResult;
				LoadMeshLODResult.Result.Key = false;
						
				Promise->SetValue(LoadMeshLODResult);
				return;
			}

			FglTFRuntimeMeshLOD& Ref = LODs.AddDefaulted_GetRef();
			USpawnGLTFMeshLibrary::LoadAssetAsLOD(Results[i].Result.Value, MeshResources[i].MeshName, Ref);
		}
		
		// temp solution till we figure out how to do this properly
		if (Results.IsEmpty())
		{
			UBF::FLoadMeshLODResult LoadMeshLODResult;
			LoadMeshLODResult.Result.Key = false;
			Promise->SetValue(LoadMeshLODResult);
		}
		else
		{
			const FglTFRuntimeLODData LODData(Results[0].Result.Value, LODs);
			UStreamableRenderAsset* Asset = USpawnGLTFMeshLibrary::LoadMeshLOD(LODData, MeshConfigData);
			UBF::FLoadMeshLODResult LoadMeshLODResult;
			LoadMeshLODResult.Result.Value = Asset;
			LoadMeshLODResult.Result.Key = true;
			
			Promise->SetValue(LoadMeshLODResult);
			// Handle Asset
		}
	});

	return Future;
}

void UGlobalArtifactProviderSubsystem::PrintBlueprintDebug(const FString& ArtifactId, const FString& ContextString)
{
	if (!Catalog.Contains(ArtifactId))
	{
		return;
	}
	
	const auto GraphResource = Catalog[ArtifactId];
	UE_LOG(LogUBF, Verbose, TEXT("Try Loading Graph %s from Uri %s"), *ArtifactId, *GraphResource.Uri);
	
	FDownloadRequestManager::GetInstance()->LoadStringFromURI(TEXT("Graph"),GraphResource.Uri, GraphResource.Hash, GraphCacheLoader)
	.Next([this, ArtifactId, ContextString](const UBF::FLoadStringResult& LoadGraphResult)
	{
		if (LoadGraphResult.Result.Key)
			UE_LOG(LogUBF, Log, TEXT("Context: %s GraphId: %s Graph: \\n\\n%s"), *ContextString, *ArtifactId, *LoadGraphResult.Result.Value);
	});
}

void UGlobalArtifactProviderSubsystem::RegisterCatalog(const UBF::FCatalogElement& CatalogElement)
{
	if (Catalog.Contains(CatalogElement.Id))
	{
		// If we are just registering the same catalog element again its ok
		// TODO maybe use new hash?
		if (Catalog[CatalogElement.Id].EqualWithoutHash(CatalogElement))
			return;

		UE_LOG(LogUBF, Warning, TEXT("[APIGraphProvider] Catalog Id Collision Detected. Existing entry: %s New Entry: %s"),
			*Catalog[CatalogElement.Id].ToString(), *CatalogElement.ToString());
		return;
	}

	Catalog.Add(CatalogElement.Id, CatalogElement);
}

void UGlobalArtifactProviderSubsystem::RegisterCatalogs(const TMap<FString, UBF::FCatalogElement>& CatalogMap)
{
	for (const auto& CatalogElement : CatalogMap)
	{
		RegisterCatalog(CatalogElement.Value);
	}
}