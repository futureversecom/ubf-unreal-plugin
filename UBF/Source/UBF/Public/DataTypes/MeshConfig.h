// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "glTFRuntimeParser.h"
#include "MeshConfig.generated.h"

/**
 * Data exists here so it can be handled in Editor
 */
USTRUCT(BlueprintType)
struct UBF_API FMeshConfigData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FglTFRuntimeSkeletalMeshConfig SkeletalMeshConfig;
	
	UPROPERTY(EditAnywhere)
	FglTFRuntimeConfig RuntimeConfig;

	UPROPERTY(EditAnywhere)
	bool bLoadAnimation = false;

	// Gets applied to SkeletalMeshConfig, Time will get used as Key
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve LODCurve;
};

namespace UBF
{
	/**
	 * Holds config details for needed for SpawnMesh node
	 */
	class FMeshConfig
	{
	public:
		FMeshConfig(const FMeshConfigData& MeshConfigData) : MeshConfigData(MeshConfigData) {}
		
		FMeshConfigData MeshConfigData;
	};

}