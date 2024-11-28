#pragma once

#include "glTFRuntimeParser.h"
#include "MeshConfig.generated.h"

/**
 * Data exists here so it can be handled in Editor
 */
USTRUCT()
struct UBF_API FMeshConfigData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FglTFRuntimeSkeletalMeshConfig SkeletalMeshConfig;
	
	UPROPERTY(EditAnywhere)
	FglTFRuntimeConfig RuntimeConfig;

	UPROPERTY(EditAnywhere)
	bool bLoadAnimation = false;
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