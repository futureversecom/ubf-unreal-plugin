// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once

#include "MeshArtifactImportSettings.generated.h"

/**
 * 
 */
USTRUCT()
struct UBF_API FMeshArtifactImportSettings
{
	GENERATED_BODY()
	FMeshArtifactImportSettings() {}
	FMeshArtifactImportSettings(const FString& MeshIdentifier) : MeshIdentifier(MeshIdentifier) {}
	
	UPROPERTY()
	FString MeshIdentifier;
};
