// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#include "GlobalArtifactProvider/CachedMesh.h"
#include "glTFRuntimeAsset.h"

namespace UBF
{
	bool FMeshImportSettings::AreBasisMatricesEqual(const FglTFRuntimeBasisMatrix& A, const FglTFRuntimeBasisMatrix& B)
	{
		return A.XAxis.Equals(B.XAxis, KINDA_SMALL_NUMBER) &&
			A.YAxis.Equals(B.YAxis, KINDA_SMALL_NUMBER) &&
			A.ZAxis.Equals(B.ZAxis, KINDA_SMALL_NUMBER) &&
			A.Origin.Equals(B.Origin, KINDA_SMALL_NUMBER);
	}

	bool FMeshImportSettings::AreImportConfigsEqual(const FglTFRuntimeConfig& A, const FglTFRuntimeConfig& B)
	{
		return A.TransformBaseType == B.TransformBaseType &&
			A.BasisMatrix.Equals(B.BasisMatrix, KINDA_SMALL_NUMBER) &&
			A.BaseTransform.Equals(B.BaseTransform, KINDA_SMALL_NUMBER) &&
			AreBasisMatricesEqual(A.BasisVectorMatrix, B.BasisVectorMatrix) &&
			FMath::IsNearlyEqual(A.SceneScale, B.SceneScale, KINDA_SMALL_NUMBER) &&
			A.ContentPluginsToScan == B.ContentPluginsToScan &&
			A.bAllowExternalFiles == B.bAllowExternalFiles &&
			A.OverrideBaseDirectory == B.OverrideBaseDirectory &&
			A.bOverrideBaseDirectoryFromContentDir == B.bOverrideBaseDirectoryFromContentDir &&
			A.ArchiveEntryPoint == B.ArchiveEntryPoint &&
			A.ArchiveAutoEntryPointExtensions == B.ArchiveAutoEntryPointExtensions &&
			A.bSearchContentDir == B.bSearchContentDir &&
			A.RuntimeContextObject == B.RuntimeContextObject &&  // Pointer comparison
			A.RuntimeContextString == B.RuntimeContextString &&
			A.bAsBlob == B.bAsBlob &&
			A.PrefixForUnnamedNodes == B.PrefixForUnnamedNodes &&
			A.EncryptionKey == B.EncryptionKey;
	}

	FString FMeshImportSettings::FglTFRuntimeConfigToString(const FglTFRuntimeConfig& Config)
	{
		FString Result;
		Result += FString::Printf(TEXT("TransformBaseType: %d\n"), static_cast<int32>(Config.TransformBaseType));
		Result += FString::Printf(TEXT("BasisMatrix: %s\n"), *Config.BasisMatrix.ToString());
		Result += FString::Printf(TEXT("BaseTransform: %s\n"), *Config.BaseTransform.ToString());
		Result += FString::Printf(TEXT("BasisVectorMatrix: %s\n"), *Config.BasisVectorMatrix.GetMatrix().ToString());
		Result += FString::Printf(TEXT("SceneScale: %f\n"), Config.SceneScale);
		Result += FString::Printf(TEXT("bAllowExternalFiles: %s\n"), Config.bAllowExternalFiles ? TEXT("True") : TEXT("False"));
		Result += FString::Printf(TEXT("OverrideBaseDirectory: %s\n"), *Config.OverrideBaseDirectory);
		Result += FString::Printf(TEXT("bOverrideBaseDirectoryFromContentDir: %s\n"), Config.bOverrideBaseDirectoryFromContentDir ? TEXT("True") : TEXT("False"));
		Result += FString::Printf(TEXT("ArchiveEntryPoint: %s\n"), *Config.ArchiveEntryPoint);
		Result += FString::Printf(TEXT("ArchiveAutoEntryPointExtensions: %s\n"), *Config.ArchiveAutoEntryPointExtensions);
		Result += FString::Printf(TEXT("bSearchContentDir: %s\n"), Config.bSearchContentDir ? TEXT("True") : TEXT("False"));
		Result += FString::Printf(TEXT("RuntimeContextObject: %s\n"), Config.RuntimeContextObject ? *Config.RuntimeContextObject->GetName() : TEXT("None"));
		Result += FString::Printf(TEXT("RuntimeContextString: %s\n"), *Config.RuntimeContextString);
		Result += FString::Printf(TEXT("bAsBlob: %s\n"), Config.bAsBlob ? TEXT("True") : TEXT("False"));
		Result += FString::Printf(TEXT("PrefixForUnnamedNodes: %s\n"), *Config.PrefixForUnnamedNodes);
		Result += FString::Printf(TEXT("EncryptionKey: %s\n"), *Config.EncryptionKey);

		// Print all content plugins
		Result += TEXT("ContentPluginsToScan:\n");
		for (const FString& Plugin : Config.ContentPluginsToScan)
		{
			Result += FString::Printf(TEXT("  - %s\n"), *Plugin);
		}

		return Result;
	}

	FCachedMeshEntry::FCachedMeshEntry(const FMeshImportSettings& ImportSettings, UglTFRuntimeAsset* Asset): ImportSettings(ImportSettings), Asset(Asset)
	{
		
	}

	bool FCachedMesh::ContainsMesh(const FMeshImportSettings& ImportSettings)
	{
		int Index = GetIndexForConfig(ImportSettings);
		
		if (!LoadedAssets.IsValidIndex(Index))
			return false;
		
		return LoadedAssets[Index].Asset.IsValid();
	}

	void FCachedMesh::AddOrReplaceMesh(const FMeshImportSettings& ImportSettings, UglTFRuntimeAsset* Asset)
	{
		int Index = GetIndexForConfig(ImportSettings);

		if (!LoadedAssets.IsValidIndex(Index))
		{
			LoadedAssets.Add(FCachedMeshEntry(ImportSettings, Asset));
			return;
		}

		LoadedAssets[Index].Asset = Asset;
	}

	UglTFRuntimeAsset* FCachedMesh::GetMesh(const FMeshImportSettings& ImportSettings)
	{
		int Index = GetIndexForConfig(ImportSettings);

		if (!LoadedAssets.IsValidIndex(Index))
			return nullptr;

		return LoadedAssets[Index].Asset.Get();
	}

	int FCachedMesh::GetIndexForConfig(const FMeshImportSettings& ImportSettings)
	{
		for (int i = 0; i < LoadedAssets.Num(); i++)
		{
			if (LoadedAssets[i].ImportSettings == ImportSettings)
				return i;
		}

		return -1;
	}
}

