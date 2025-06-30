// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CachedMesh.h"
#include "CatalogElement.h"
#include "GraphProvider.h"
#include "DataTypes/MeshConfig.h"
#include "DataTypes/MeshResource.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalArtifactProviderSubsystem.generated.h"

class UURIResolverBase;
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
	virtual TFuture<UBF::FLoadMeshLODResult> GetMeshLODResource(const TArray<FMeshResource>& MeshResources, const FMeshConfigData& MeshConfigData);

	UFUNCTION(BlueprintCallable)
	void GetAllLoadedBlueprintIDs(TArray<FString>& BlueprintIds) const;
	UBF::FCatalogElement GetCatalogInfo(const FString& ArtifactId) const;
	
	virtual void PrintBlueprintDebug(const FString& ArtifactId, const FString& ContextString);

	void RegisterCatalog(const UBF::FCatalogElement& CatalogElement);
	void RegisterCatalogs(const TMap<FString, UBF::FCatalogElement>& CatalogMap);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
private:
	TFuture<UBF::FLoadDataArrayResult> LoadDataFromURI(const FString& TypeID, const FString& URI, const FString& Hash, const TSharedPtr<ICacheLoader>& Cache) const;
	TFuture<UBF::FLoadStringResult> LoadStringFromURI(const FString& TypeID, const FString& URI, const FString& Hash, const TSharedPtr<ICacheLoader>& Cache);
	
	UURIResolverBase* GetResolverForURI(const FString& URI) const;

	void ClearFromUnrealCaches(const UBF::FCatalogElement& CatalogElement);
	
	TMap<FString, TWeakObjectPtr<UTexture2D>> LoadedTexturesMap;
	TMap<FString, UBF::FCachedMesh> LoadedMeshesMap;
	
	TMap<FString, UBF::FCatalogElement> Catalog;

	TSharedPtr<ICacheLoader> GraphCacheLoader;
	TSharedPtr<ICacheLoader> ResourceCacheLoader;

	UPROPERTY()
	TArray<UURIResolverBase*> Resolvers;
};
