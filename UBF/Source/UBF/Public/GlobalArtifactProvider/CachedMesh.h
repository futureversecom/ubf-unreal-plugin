// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once
#include "glTFRuntimeParser.h"


class UglTFRuntimeAsset;

struct FMeshImportSettings
{
	FMeshImportSettings() {}
	FMeshImportSettings(const FglTFRuntimeConfig& Config) : ImportConfig(Config) {}
	
	FglTFRuntimeConfig ImportConfig;

	// Custom comparison function for FglTFRuntimeBasisMatrix
	static bool AreBasisMatricesEqual(const FglTFRuntimeBasisMatrix& A, const FglTFRuntimeBasisMatrix& B);

	// Custom comparison function for FglTFRuntimeConfig
	static bool AreImportConfigsEqual(const FglTFRuntimeConfig& A, const FglTFRuntimeConfig& B);

	static FString FglTFRuntimeConfigToString(const FglTFRuntimeConfig& Config);

	// Equality operator comparing only ImportConfig
	bool operator==(const FMeshImportSettings& Other) const
	{
		return AreImportConfigsEqual(ImportConfig, Other.ImportConfig);
	}

	bool operator!=(const FMeshImportSettings& Other) const
	{
		return !(*this == Other);
	}
};

struct FCachedMeshEntry
{
	FCachedMeshEntry() {}
	FCachedMeshEntry(const FMeshImportSettings& ImportConfig, UglTFRuntimeAsset* Asset);

	FMeshImportSettings ImportSettings;
	TWeakObjectPtr<UglTFRuntimeAsset> Asset;

	// Equality operator comparing only ImportConfig
	bool operator==(const FCachedMeshEntry& Other) const
	{
		return ImportSettings == Other.ImportSettings;
	}

	bool operator!=(const FCachedMeshEntry& Other) const
	{
		return !(*this == Other);
	}
};

struct FCachedMesh
{
	FCachedMesh() {}

	bool ContainsMesh(const FglTFRuntimeConfig& Config);
	void AddOrReplaceMesh(const FglTFRuntimeConfig& Config, UglTFRuntimeAsset* Asset);
	UglTFRuntimeAsset* GetMesh(const FglTFRuntimeConfig& Config);

private:
	int GetIndexForConfig(const FglTFRuntimeConfig& Config);

	
	TArray<FCachedMeshEntry> LoadedAssets;
};