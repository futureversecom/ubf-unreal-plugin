// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/MeshConfig.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpawnGLTFMeshLibrary.generated.h"

class UglTFRuntimeAsset;
/**
 * 
 */
UCLASS()
class UBF_API USpawnGLTFMeshLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static AActor* SpawnMesh(UObject* WorldContext, UglTFRuntimeAsset* Asset, const FMeshConfigData& MeshConfig);
	
	static AActor* SpawnLODMesh(UObject* WorldContext, UStreamableRenderAsset* LODMesh);

	static UStreamableRenderAsset* LoadMeshLOD(const TArray<FglTFRuntimeMeshLOD>& LODs, const FMeshConfigData& MeshConfig);

	static bool LoadAssetAsLOD(UglTFRuntimeAsset* Asset, const FString& MeshName, FglTFRuntimeMeshLOD& OutLOD);
};
