// Copyright (c) 2025, Futureverse Corporation Limited. All rights reserved.

#pragma once
#include "TextureArtifactImportSettings.generated.h"

USTRUCT()
struct UBF_API FTextureArtifactImportSettings
{
	GENERATED_BODY()

	UPROPERTY()
	bool IsSrgb = false;
};
