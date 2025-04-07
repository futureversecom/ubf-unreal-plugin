// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CachedMesh.h"
#include "CatalogElement.h"
#include "GraphProvider.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalArtifactProviderSubsystem.generated.h"

class ICacheLoader;
/**
 * 
 */
UCLASS()
class UBF_API UGlobalArtifactProviderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UGlobalArtifactProviderSubsystem();

	static UGlobalArtifactProviderSubsystem* Get(const UObject* WorldContext);
	
	virtual TFuture<UBF::FLoadGraphResult> GetGraph(const FString& ArtifactId);
	
	virtual TFuture<UBF::FLoadTextureResult> GetTextureResource(const FString& ArtifactId);

	virtual TFuture<UBF::FLoadMeshResult> GetMeshResource(const FString& ArtifactId, const UBF::FMeshImportSettings& ImportSettings);
	virtual void PrintBlueprintDebug(const FString& ArtifactId, const FString& ContextString);

	void RegisterCatalog(const UBF::FCatalogElement& CatalogElement);
	void RegisterCatalogs(const TMap<FString, UBF::FCatalogElement>& CatalogMap);
	void RegisterBlueprintJson(const FBlueprintJson& BlueprintJson);

private:

	TMap<FString, TWeakObjectPtr<UTexture2D>> LoadedTexturesMap;
	TMap<FString, UBF::FCachedMesh> LoadedMeshesMap;
	
	TMap<FString, UBF::FCatalogElement> Catalog;
	TMap<FString, FBlueprintJson> BlueprintJsons;

	TSharedPtr<ICacheLoader> GraphCacheLoader;
	TSharedPtr<ICacheLoader> ResourceCacheLoader;
};
