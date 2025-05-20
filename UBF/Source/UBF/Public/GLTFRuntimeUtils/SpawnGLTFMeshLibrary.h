// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/MeshConfig.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpawnGLTFMeshLibrary.generated.h"

class UglTFRuntimeAsset;

USTRUCT(BlueprintType)
struct FglTFRuntimeLODData
{
	GENERATED_BODY()
	
	FglTFRuntimeLODData(): ParsingAsset(nullptr) {}

	FglTFRuntimeLODData(UglTFRuntimeAsset* Asset, const TArray<FglTFRuntimeMeshLOD>& LODs)
	: ParsingAsset(Asset), LODs(LODs) {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UglTFRuntimeAsset* ParsingAsset;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FglTFRuntimeMeshLOD> LODs;
};

/**
 * 
 */
UCLASS()
class UBF_API USpawnGLTFMeshLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContext"))
	static AActor* SpawnMesh(UObject* WorldContext, UglTFRuntimeAsset* Asset, const FMeshConfigData& MeshConfig);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContext"))
	static AActor* SpawnLODMesh(UObject* WorldContext, UStreamableRenderAsset* LODMesh);

	UFUNCTION(BlueprintCallable)
	static UStreamableRenderAsset* LoadMeshLOD(const FglTFRuntimeLODData& LODData, const FMeshConfigData& MeshConfig);

	UFUNCTION(BlueprintCallable)
	static bool LoadAssetAsLOD(UglTFRuntimeAsset* Asset, const FString& MeshName, FglTFRuntimeMeshLOD& OutLOD);
};
