// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.


#include "GlobalArtifactProvider/GlobalArtifactProviderSubsystem.h"

#include "glTFRuntimeAsset.h"
#include "ImageUtils.h"
#include "JsonObjectConverter.h"
#include "Registry.h"
#include "GlobalArtifactProvider/DownloadRequestManager.h"
#include "GlobalArtifactProvider/GlobalArtifactProviderSettings.h"
#include "GlobalArtifactProvider/CacheLoading/TempCacheLoader.h"
#include "GlobalArtifactProvider/CacheLoading/MemoryCacheLoader.h"
#include "GlobalArtifactProvider/ImportSettings/MeshArtifactImportSettings.h"
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

	// get graph from catalog using graph instance's blueprint id
	if (!Catalog.Contains(ArtifactId))
	{
		UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetGraph Failed to get graph because no graph catalog element found for ArtifactId: %s"), *ArtifactId);
		UBF::FLoadGraphResult LoadResult;
		Promise->SetValue(LoadResult);
		return Future;
	}
	
	const auto GraphResource = Catalog[ArtifactId];
	UE_LOG(LogUBF, Verbose, TEXT("Try Loading Graph %s from Uri %s"), *ArtifactId, *GraphResource.Uri);
	
	LoadStringFromURI(TEXT("Graph"), GraphResource.Uri, GraphResource.Hash, GraphCacheLoader)
	.Next([this, ArtifactId, Promise](const UBF::FLoadStringResult& LoadGraphResult)
	{
		UBF::FLoadGraphResult PromiseResult;
	
		if (!LoadGraphResult.bSuccess)
		{
			Promise->SetValue(PromiseResult);	
			return;
		}
	
		UBF::FGraphHandle Graph;
		if (UBF::FGraphHandle::Load(UBF::FRegistryHandle::Default(), LoadGraphResult.Value, Graph))
		{
			UE_LOG(LogUBF, Verbose, TEXT("Successfully loaded Graph %s"), *ArtifactId);
		
			PromiseResult.bSuccess = true;
			PromiseResult.Value = Graph;
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
		LoadResult.bSuccess = true;
		LoadResult.Value = LoadedTexturesMap[ArtifactId].Get();
		Promise->SetValue(LoadResult);
		return Future;
	}
	
	if (!Catalog.Contains(ArtifactId))
	{
		UE_LOG(LogUBF, Verbose, TEXT("UGlobalArtifactProviderSubsystem::GetTextureResource UBF doesn't have a ResourceId '%s' entry"), *ArtifactId);
		Promise->SetValue(UBF::FLoadTextureResult());
		return Future;
	}

	const auto ResourceManifestElement = Catalog[ArtifactId];
	LoadDataFromURI(TEXT("Texture"),ResourceManifestElement.Uri, ResourceManifestElement.Hash, ResourceCacheLoader)
	.Next([this, Promise, ArtifactId](const UBF::FLoadDataArrayResult& DataResult)
	{
		TSharedPtr<TArray<uint8>> Data = DataResult.Value;
		UBF::FLoadTextureResult LoadResult;
		
		if (!DataResult.bSuccess)
		{
			UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetTextureResource data load failed"));
			Promise->SetValue(LoadResult);
			return;
		}
		
		if (!Data.IsValid() || Data->Num() == 0 || Data->GetData() == nullptr)
		{
			UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetTextureResource Data is empty"));
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
			Texture = FImageUtils::ImportBufferAsTexture2D(*Data);
			LoadedTexturesMap.Remove(ArtifactId);
			LoadedTexturesMap.Add(ArtifactId, Texture);
		}
		
		LoadResult.Value = Texture;
		LoadResult.bSuccess = true;
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
		UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource UBF doesn't have a ResourceId %s entry."), *ArtifactId);
		Promise->SetValue(UBF::FLoadMeshResult());
		return Future;
	}

	const auto ResourceManifestElement = Catalog[ArtifactId];
	const auto Uri = ResourceManifestElement.Uri;
	if (LoadedMeshesMap.Contains(Uri) && LoadedMeshesMap[Uri].ContainsMesh(ImportSettings))
	{
		UBF::FLoadMeshResult LoadResult;
		LoadResult.bSuccess = true;
		LoadResult.Value = LoadedMeshesMap[Uri].GetMesh(ImportSettings);
		Promise->SetValue(LoadResult);
		return Future;
	}
	
	LoadDataFromURI(TEXT("Mesh"),ResourceManifestElement.Uri, ResourceManifestElement.Hash, ResourceCacheLoader)
	.Next([this, Promise, ImportSettings, ArtifactId, Uri](const UBF::FLoadDataArrayResult& DataResult)
	{
		TSharedPtr<TArray<uint8>> Data = DataResult.Value;
		UBF::FLoadMeshResult LoadResult;
		
		if (!DataResult.bSuccess)
		{
			UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource data load failed"));
			Promise->SetValue(LoadResult);
			return;
		}
			
		if (!Data.IsValid() || Data->Num() == 0 || Data->GetData() == nullptr)
		{
			UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource Data is empty"));
			Promise->SetValue(LoadResult);
			return;
		}

		UglTFRuntimeAsset* Asset;

		if (LoadedMeshesMap.Contains(Uri) && LoadedMeshesMap[Uri].ContainsMesh(ImportSettings))
		{
			Asset = LoadedMeshesMap[Uri].GetMesh(ImportSettings);
			UE_LOG(LogUBF, Verbose, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource Found Cached Mesh for %s"), *Uri);
		}
		else
		{
			TRACE_CPUPROFILER_EVENT_SCOPE(UGlobalArtifactProviderSubsystem::GetMeshResource_ImportDataAsGLTFRuntimeAsset);

			Asset = NewObject<UglTFRuntimeAsset>();
			Asset->RuntimeContextObject = ImportSettings.ImportConfig.RuntimeContextObject;
			Asset->RuntimeContextString = ImportSettings.ImportConfig.RuntimeContextString;
					
			if (!Asset->LoadFromData(Data->GetData(), Data->Num(), ImportSettings.ImportConfig))
			{
				UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource Failed to Load Mesh from Data %s"), *ArtifactId);
				Promise->SetValue(LoadResult);
				return;
			}
			
			LoadedMeshesMap.FindOrAdd(Uri).AddOrReplaceMesh(ImportSettings, Asset);
			UE_LOG(LogUBF, VeryVerbose, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource Caching New Mesh for %s"), *Uri);
		}
		
		LoadResult.bSuccess = true;
		LoadResult.Value = Asset;
		
		Promise->SetValue(LoadResult);
	});

	return Future;
}

TFuture<UBF::FLoadMeshLODResult> UGlobalArtifactProviderSubsystem::GetMeshLODResource(
	const TArray<FString>& ArtifactIds, const FMeshConfigData& MeshConfigData)
{
	// TODO cache end results
	
	TSharedPtr<TPromise<UBF::FLoadMeshLODResult>> Promise = MakeShareable(new TPromise<UBF::FLoadMeshLODResult>());
	TFuture<UBF::FLoadMeshLODResult> Future = Promise->GetFuture();
	
	// Load all mesh resources
	TArray<TFuture<UBF::FLoadMeshResult>> MeshFutures;
	TArray<FString> MeshIds;
	for (const FString& ArtifactId : ArtifactIds)
	{
		MeshFutures.Add(GetMeshResource(ArtifactId, UBF::FMeshImportSettings(MeshConfigData.RuntimeConfig)));
		
		if (Catalog.Contains(ArtifactId))
		{
			const auto MeshArtifact = Catalog[ArtifactId];
			if (!MeshArtifact.MetadataJsonWrapper.JsonString.IsEmpty())
			{
				FMeshArtifactImportSettings MeshArtifactImportSettings;
				FJsonObjectConverter::JsonObjectStringToUStruct(MeshArtifact.MetadataJsonWrapper.JsonString, &MeshArtifactImportSettings);
				MeshIds.Add(MeshArtifactImportSettings.MeshIdentifier);
				UE_LOG(LogUBF, Verbose, TEXT("UGlobalArtifactProviderSubsystem::GetMeshLODResource Found MeshId metadata %s."), *MeshArtifactImportSettings.MeshIdentifier);
			}
		}
	}
	
	WaitAll(MeshFutures).Next([this, Promise, MeshConfigData, MeshIds](const TArray<UBF::FLoadMeshResult>& Results)
	{
		ensure(MeshIds.Num() == Results.Num());
		
		TArray<FglTFRuntimeMeshLOD> LODs;

		for (int i = 0; i < Results.Num(); i++)
		{
			if (!Results[i].bSuccess)
			{
				UBF::FLoadMeshLODResult LoadMeshLODResult;
						
				Promise->SetValue(LoadMeshLODResult);
				return;
			}

			FglTFRuntimeMeshLOD& Ref = LODs.AddDefaulted_GetRef();
			if (!USpawnGLTFMeshLibrary::LoadAssetAsLOD(Results[i].Value, MeshIds[i], Ref))
			{
				UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::GetMeshResource Failed to Load Asset As LOD for %s"), *MeshIds[i]);
				UBF::FLoadMeshLODResult LoadMeshLODResult;
				Promise->SetValue(LoadMeshLODResult);
				return;
			}
		}
		
		// temp solution till we figure out how to do this properly
		UStreamableRenderAsset* Asset = USpawnGLTFMeshLibrary::LoadMeshLOD(FglTFRuntimeLODData(Results.Last().Value, LODs), MeshConfigData);

		UBF::FLoadMeshLODResult LoadMeshLODResult;
		if (!Asset)
		{
			Promise->SetValue(LoadMeshLODResult);
		}
		else
		{
			LoadMeshLODResult.SetResult(Asset);
			Promise->SetValue(LoadMeshLODResult);
			// Handle Asset
		}
	});

	return Future;
}

void UGlobalArtifactProviderSubsystem::GetAllLoadedBlueprintIDs(TArray<FString>& Ids) const
{
	Ids.Reset();
	
	for (const auto& Element : Catalog)
	{
		if (Element.Value.Type.ToLower() == TEXT("blueprint"))
			Ids.Add(Element.Value.Id);
	}
}

UBF::FCatalogElement UGlobalArtifactProviderSubsystem::GetCatalogInfo(const FString& ArtifactId) const
{
	return Catalog.FindRef(ArtifactId);
}

void UGlobalArtifactProviderSubsystem::PrintBlueprintDebug(const FString& ArtifactId, const FString& ContextString)
{
	if (!Catalog.Contains(ArtifactId))
	{
		return;
	}
	
	const auto GraphResource = Catalog[ArtifactId];
	UE_LOG(LogUBF, Verbose, TEXT("Try Loading Graph %s from Uri %s"), *ArtifactId, *GraphResource.Uri);
	
	LoadStringFromURI(TEXT("Graph"),GraphResource.Uri, GraphResource.Hash, GraphCacheLoader)
	.Next([this, ArtifactId, ContextString](const UBF::FLoadStringResult& LoadGraphResult)
	{
		if (LoadGraphResult.bSuccess)
			UE_LOG(LogUBF, Log, TEXT("Context: %s GraphId: %s Graph: \\n\\n%s"), *ContextString, *ArtifactId, *LoadGraphResult.Value);
	});
}

void UGlobalArtifactProviderSubsystem::RegisterCatalog(const UBF::FCatalogElement& CatalogElement)
{
	if (Catalog.Contains(CatalogElement.Id))
	{
		// If it matches with has just ignore
		if (Catalog[CatalogElement.Id] == CatalogElement)
		{
			return;
		}
		
		// If we are just registering the same catalog element again its ok
		if (Catalog[CatalogElement.Id].EqualWithoutHash(CatalogElement))
		{
			ClearFromUnrealCaches(Catalog[CatalogElement.Id]);
			Catalog[CatalogElement.Id] = CatalogElement;
			OnCatalogElementChanged.Broadcast(CatalogElement.Id);
			return;
		}

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

void UGlobalArtifactProviderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (const UGlobalArtifactProviderSettings* Settings = GetDefault<UGlobalArtifactProviderSettings>())
	{
		for (auto SoftClassPtr : Settings->URIResolvers)
		{
			UClass* ResolverClass = SoftClassPtr.TryLoadClass<UURIResolverBase>();
			if (!ResolverClass) continue;
			if (ResolverClass->HasAnyClassFlags(CLASS_Abstract)) continue;

			UURIResolverBase* Resolver = NewObject<UURIResolverBase>(this, ResolverClass);

			if (!Resolver) continue;

			Resolvers.Add(Resolver);
		}

		if (Resolvers.Num() == 0)
		{
			UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem no resolvers provided, unable to resolve any URIs. These can be added in project settings"));
		}
	}
}

TFuture<UBF::FLoadDataArrayResult> UGlobalArtifactProviderSubsystem::LoadDataFromURI(const FString& TypeID,
	const FString& URI, const FString& Hash, const TSharedPtr<ICacheLoader>& Cache) const
{
	TSharedPtr<TPromise<UBF::FLoadDataArrayResult>> LoadResult = MakeShared<TPromise<UBF::FLoadDataArrayResult>>();
	TSharedPtr<TArray<uint8>> Data = MakeShared<TArray<uint8>>();
	
	if (Cache.IsValid() && Cache->TryGetCachedBytes(URI, Hash, *Data))
	{
		UBF::FLoadDataArrayResult Result;
		Result.bSuccess = true;
		Result.Value = Data;
		LoadResult->SetValue(Result);
		return LoadResult->GetFuture();
	}

	UURIResolverBase* Resolver = GetResolverForURI(URI);

	if (!Resolver)
	{
		UE_LOG(LogUBF, Error, TEXT("UGlobalArtifactProviderSubsystem::LoadDataFromURI failed to find resolver for URI %s cannot load data"), *URI);
		LoadResult->SetValue(UBF::FLoadDataArrayResult());
		return LoadResult->GetFuture();
	}

	Resolver->ResolveURI(TypeID, URI).Next([LoadResult, URI, Hash, Cache](const UBF::FLoadDataArrayResult& ResolveResult)
	{
		if (Cache.IsValid() && ResolveResult.bSuccess && ResolveResult.Value.IsValid())
			Cache->CacheBytes(URI, Hash, *ResolveResult.Value);
		LoadResult->SetValue(ResolveResult);
	});

	return LoadResult->GetFuture();
}

TFuture<UBF::FLoadStringResult> UGlobalArtifactProviderSubsystem::LoadStringFromURI(const FString& TypeID,
	const FString& URI, const FString& Hash, const TSharedPtr<ICacheLoader>& Cache)
{
	TSharedPtr<TPromise<UBF::FLoadStringResult>> Promise = MakeShared<TPromise<UBF::FLoadStringResult>>();
	
	LoadDataFromURI(TypeID, URI, Hash, Cache).Next([Promise](const UBF::FLoadDataArrayResult& Result)
	{
		if (!Result.bSuccess)
		{
			Promise->SetValue(UBF::FLoadStringResult());
			return;
		}

		UBF::FLoadStringResult StringResult;
		StringResult.bSuccess = true;
		
		StringResult.Value = FString(StringCast<TCHAR>(reinterpret_cast<const char*>(Result.Value->GetData()), Result.Value->Num()));
		
		Promise->SetValue(StringResult);
	});

	return Promise->GetFuture();
}

UURIResolverBase* UGlobalArtifactProviderSubsystem::GetResolverForURI(const FString& URI) const
{
	for (auto Resolver : Resolvers)
	{
		if (!ensure(Resolver)) continue;

		if (Resolver->CanResolveURI(URI))
			return Resolver;
	}

	return nullptr;
}

void UGlobalArtifactProviderSubsystem::ClearFromUnrealCaches(const UBF::FCatalogElement& CatalogElement)
{
	LoadedTexturesMap.Remove(CatalogElement.Id);
	LoadedMeshesMap.Remove(CatalogElement.Id);
}
